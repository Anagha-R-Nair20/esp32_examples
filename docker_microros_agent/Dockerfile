# Use the micro-ROS ESP-IDF image as base
FROM microros/micro-ros-agent:humble


# Install the rmw_cyclonedds_cpp RMW implementation
#RUN apt-get update && apt-get install -y \
#    ros-humble-rmw-cyclonedds-cpp \
 #   && rm -rf /var/lib/apt/lists/*

RUN apt-get update 
RUN useradd -ms /bin/bash uros_agent
WORKDIR /agent
USER uros_agent


# Start bash shell
# CMD ["tail -f /dev/null"]
