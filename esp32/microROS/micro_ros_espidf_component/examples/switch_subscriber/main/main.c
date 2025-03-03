#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include <uros_network_interfaces.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/int32_multi_array.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#ifdef CONFIG_MICRO_ROS_ESP_XRCE_DDS_MIDDLEWARE
#include <rmw_microros/rmw_microros.h>
#endif

// Define GPIO pins for relay module
#define RELAY1_PIN 5
#define RELAY2_PIN 18
#define RELAY3_PIN 19

// Macros for error checking.
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)) { printf("Failed status on line %d: %d. Aborting.\n", __LINE__, (int)temp_rc); vTaskDelete(NULL); } }
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)) { printf("Failed status on line %d: %d. Continuing.\n", __LINE__, (int)temp_rc); } }

// Global subscriber object and message.
rcl_subscription_t subscriber;
std_msgs__msg__Int32MultiArray led_msg;

// Subscription callback to control relays
void subscription_callback(const void * msgin) {
    const std_msgs__msg__Int32MultiArray * msg = (const std_msgs__msg__Int32MultiArray *)msgin;
    if (msg->data.size >= 3) {
        int relay1 = msg->data.data[0];
        int relay2 = msg->data.data[1];
        int relay3 = msg->data.data[2];

        printf("Received Relay States: %d, %d, %d\n", relay1, relay2, relay3);

        gpio_set_level(RELAY1_PIN, relay1 ? 1 : 0);
        gpio_set_level(RELAY2_PIN, relay2 ? 1 : 0);
        gpio_set_level(RELAY3_PIN, relay3 ? 1 : 0);
    } else {
        printf("Invalid data received\n");
    }
}

void micro_ros_task(void * arg) {
    // Configure relay GPIOs as output
    gpio_config_t io_conf = {
        .pin_bit_mask = ((1ULL << RELAY1_PIN) | (1ULL << RELAY2_PIN) | (1ULL << RELAY3_PIN)),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Initialize micro-ROS
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    RCCHECK(rcl_init_options_init(&init_options, allocator));

#ifdef CONFIG_MICRO_ROS_ESP_XRCE_DDS_MIDDLEWARE
    rmw_init_options_t * rmw_options = rcl_init_options_get_rmw_init_options(&init_options);
    RCCHECK(rmw_uros_options_set_udp_address(CONFIG_MICRO_ROS_AGENT_IP, CONFIG_MICRO_ROS_AGENT_PORT, rmw_options));
#endif

    RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

    // Create a node
    rcl_node_t node = rcl_get_zero_initialized_node();
    RCCHECK(rclc_node_init_default(&node, "relay_subscriber", "", &support));

    // Create a subscription for the "bulb_control" topic
    RCCHECK(rclc_subscription_init_default(
        &subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
        "bulb_control"
    ));

    // Create an executor to handle incoming messages
    rclc_executor_t executor = rclc_executor_get_zero_initialized_executor();
    RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
    RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &led_msg, &subscription_callback, ON_NEW_DATA));

    while(1) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
        usleep(100000);
    }

    // Cleanup (not usually reached)
    RCCHECK(rcl_subscription_fini(&subscriber, &node));
    RCCHECK(rcl_node_fini(&node));
    vTaskDelete(NULL);
}

void app_main(void) {
#if defined(CONFIG_MICRO_ROS_ESP_NETIF_WLAN) || defined(CONFIG_MICRO_ROS_ESP_NETIF_ENET)
    ESP_ERROR_CHECK(uros_network_interface_initialize());
#endif

    xTaskCreate(micro_ros_task, "micro_ros_task", 8000, NULL, 5, NULL);
}
