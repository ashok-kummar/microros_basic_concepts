#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <sensor_msgs/msg/imu.h>

sensor_msgs__msg__Imu imu_msg;

rcl_subscription_t subscriber;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define LED_PIN 13

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

char *ssid = "";
char *pwd = "";
char *host_ip = "192.168.1.xx";

void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void subscription_callback(const void * msgin)
{  
  const sensor_msgs__msg__Imu * imu_msg = (const sensor_msgs__msg__Imu *)msgin;
  Serial.println("Linear Acceleration: ");
  Serial.println("X: " + String(imu_msg->linear_acceleration.x) + "\t" + "Y: " + String(imu_msg->linear_acceleration.y) + "\t" + "Z: " + String(imu_msg->linear_acceleration.z));

  Serial.println("Angular Velocity: ");
  Serial.println("X: " + String(imu_msg->angular_velocity.x) + "\t" + "Y: " + String(imu_msg->angular_velocity.y) + "\t" + "Z: " + String(imu_msg->angular_velocity.z));

}

void setup() {
  Serial.begin(115200);
//   set_microros_transports();
    set_microros_wifi_transports(ssid, pwd, host_ip, 8889);
  
  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "imu_subscriber_node", "", &support));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
    "imu_info_topic"));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &imu_msg, &subscription_callback, ON_NEW_DATA));
}

void loop() {
  delay(100);
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}