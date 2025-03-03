# Use the micro-ROS Agent official image as base
FROM microros/micro-ros-agent:humble

# Install additional dependencies
RUN apt-get update && apt-get install -y \
    git wget qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools \
    python3-rosdep \
    python3-vcstool \
    python3-colcon-common-extensions

# Set up a new user (Optional: micro-ROS should work as root too)
RUN useradd -ms /bin/bash uros_agent
WORKDIR /agent

# Source ROS before running commands
ENTRYPOINT ["/bin/bash", "-c", "source /opt/ros/humble/setup.bash && exec $@"]
