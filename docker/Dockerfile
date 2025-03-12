# Use the micro-ROS ESP-IDF image as base
FROM microros/esp-idf-microros:latest

RUN apt-get update 



# Set the working directory in the Docker container
WORKDIR /micro_ros_espidf_component

# Copy the local directory to the container
COPY . /micro_ros_espidf_component

# Copy the timezone file
#COPY /etc/timezone /etc/timezone
ENV TZ=UTC
RUN sudo ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ | sudo tee /etc/timezone#

# Set the user to 'espidf'
USER espidf

# Give the container full access to host devices
RUN sudo chmod -R a+rw /dev

RUN echo "source /opt/esp/idf/export.sh" >> ~/.bashrc


# Run the ESP-IDF build commands

# Start bash shell
CMD ["/bin/bash"]
