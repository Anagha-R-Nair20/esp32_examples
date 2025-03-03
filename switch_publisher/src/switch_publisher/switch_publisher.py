import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32MultiArray

class SwitchPublisher(Node):
    def __init__(self):
        super().__init__('switch_publisher')
        self.publisher_ = self.create_publisher(Int32MultiArray, 'bulb_control', 10)
        self.timer = self.create_timer(2.0, self.publish_switch_states)
        self.switch_states = [1, 0, 0]  # Start with switch 1 ON
        self.current_switch = 0  # Tracks which switch is ON

    def publish_switch_states(self):
        self.switch_states = [0, 0, 0]  # Turn off all switches
        self.switch_states[self.current_switch] = 1  # Turn ON the current switch

        msg = Int32MultiArray()
        msg.data = self.switch_states
        self.publisher_.publish(msg)
        self.get_logger().info(f"Publishing Switch States: {msg.data}")

        self.current_switch = (self.current_switch + 1) % 3  # Move to the next switch

def main(args=None):
    rclpy.init(args=args)
    node = SwitchPublisher()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
