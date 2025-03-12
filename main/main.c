#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"

#include <uros_network_interfaces.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <sensor_msgs/msg/imu.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#ifdef CONFIG_MICRO_ROS_ESP_XRCE_DDS_MIDDLEWARE
#include <rmw_microros/rmw_microros.h>
#endif

#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define MPU6050_ADDR                0x68
#define MPU6050_PWR_MGMT_1_REG      0x6B
#define MPU6050_ACCEL_XOUT_H_REG    0x3B
#define MPU6050_GYRO_XOUT_H_REG     0x43

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

rcl_publisher_t publisher;
sensor_msgs__msg__Imu imu_msg;

void i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void mpu6050_init()
{
    uint8_t wakeup_cmd = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1_REG, true);
    i2c_master_write_byte(cmd, wakeup_cmd, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void mpu6050_read_accel_gyro(float *accel_data, float *gyro_data)
{
    uint8_t data[14];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H_REG, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 14, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    int16_t accel_raw[3], gyro_raw[3];
    accel_raw[0] = (data[0] << 8) | data[1];
    accel_raw[1] = (data[2] << 8) | data[3];
    accel_raw[2] = (data[4] << 8) | data[5];
    gyro_raw[0] = (data[8] << 8) | data[9];
    gyro_raw[1] = (data[10] << 8) | data[11];
    gyro_raw[2] = (data[12] << 8) | data[13];

    for (int i = 0; i < 3; i++) {
        accel_data[i] = accel_raw[i] / 16384.0;
        gyro_data[i] = gyro_raw[i] / 131.0;
    }
}

void micro_ros_task(void * arg)
{
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;

    // Create init_options.
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    RCCHECK(rcl_init_options_init(&init_options, allocator));

#ifdef CONFIG_MICRO_ROS_ESP_XRCE_DDS_MIDDLEWARE
    rmw_init_options_t* rmw_options = rcl_init_options_get_rmw_init_options(&init_options);

    // Static Agent IP and port can be used instead of autodiscovery.
    RCCHECK(rmw_uros_options_set_udp_address(CONFIG_MICRO_ROS_AGENT_IP, CONFIG_MICRO_ROS_AGENT_PORT, rmw_options));
    //RCCHECK(rmw_uros_discover_agent(rmw_options));
#endif

    // Setup support structure.
    RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

    rcl_node_t node = rcl_get_zero_initialized_node();
    RCCHECK(rclc_node_init_default(&node, "mpu6050_publisher_rclc", "", &support));
    RCCHECK(rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        "mpu6050_data"));

    imu_msg.header.frame_id.data = "mpu6050_frame";
    imu_msg.header.frame_id.size = strlen(imu_msg.header.frame_id.data);
    imu_msg.header.frame_id.capacity = imu_msg.header.frame_id.size + 1;

    while(1){
        float accel_data[3], gyro_data[3];
        mpu6050_read_accel_gyro(accel_data, gyro_data);

        imu_msg.linear_acceleration.x = accel_data[0];
        imu_msg.linear_acceleration.y = accel_data[1];
        imu_msg.linear_acceleration.z = accel_data[2];
        imu_msg.angular_velocity.x = gyro_data[0];
        imu_msg.angular_velocity.y = gyro_data[1];
        imu_msg.angular_velocity.z = gyro_data[2];

        RCCHECK(rcl_publish(&publisher, &imu_msg, NULL));
        usleep(100000);
    }

    RCCHECK(rcl_publisher_fini(&publisher, &node));
    RCCHECK(rcl_node_fini(&node));
    vTaskDelete(NULL);
}

void app_main(void)
{
#if defined(CONFIG_MICRO_ROS_ESP_NETIF_WLAN) || defined(CONFIG_MICRO_ROS_ESP_NETIF_ENET)
    ESP_ERROR_CHECK(uros_network_interface_initialize());
#endif

    i2c_master_init();
    mpu6050_init();

    // Define stack size and task priority if not defined in menuconfig
    #ifndef CONFIG_MICRO_ROS_APP_STACK
    #define CONFIG_MICRO_ROS_APP_STACK 4096
    #endif

    #ifndef CONFIG_MICRO_ROS_APP_TASK_PRIO
    #define CONFIG_MICRO_ROS_APP_TASK_PRIO 5
    #endif
    
    // Pin micro-ros task in APP_CPU to make PRO_CPU to deal with wifi:
    xTaskCreate(micro_ros_task,
            "uros_task",
            CONFIG_MICRO_ROS_APP_STACK,
            NULL,
            CONFIG_MICRO_ROS_APP_TASK_PRIO,
            NULL);
}