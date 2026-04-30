#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>

double minimum_mesafe = 999.0;
double guvenli_mesafe = 0.6; 

void lidarCallback(const sensor_msgs::LaserScan::ConstPtr& msg) {
    minimum_mesafe = 999.0;
    int num_readings = msg->ranges.size();
    int baslangic = 330; 
    int bitis = 30;      

    for (int i = 0; i < num_readings; i++) {
        if (i < bitis || i > baslangic) {
            if (!std::isnan(msg->ranges[i]) && !std::isinf(msg->ranges[i])) {
                if (msg->ranges[i] < minimum_mesafe) {
                    minimum_mesafe = msg->ranges[i];
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "kasif_node");
    ros::NodeHandle nh;

    ros::Subscriber lidar_sub = nh.subscribe("/scan", 10, lidarCallback);
    ros::Publisher cmd_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Rate rate(10); 
    geometry_msgs::Twist move_cmd;

    ROS_INFO("Otonom gezinme yapiliyor...");

    while (ros::ok()) {
        ros::spinOnce();

        if (minimum_mesafe < guvenli_mesafe) {
            move_cmd.linear.x = 0.0;
            move_cmd.angular.z = 0.5; 
        } else {
            move_cmd.linear.x = 0.2; 
            move_cmd.angular.z = 0.0;
        }
        cmd_pub.publish(move_cmd);
        rate.sleep();
    }
    return 0;
}
