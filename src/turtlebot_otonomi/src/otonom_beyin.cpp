#include <ros/ros.h>
#include <nav_msgs/OccupancyGrid.h>
#include <geometry_msgs/Twist.h>
#include <stdlib.h>

class OtonomBeyin {
private:
    ros::NodeHandle nh;
    ros::Subscriber map_sub;
    ros::Publisher cmd_pub;
    int last_area;
    ros::Time unchanged_time;
    bool is_done;

public:
    OtonomBeyin() : last_area(0), is_done(false) {
        map_sub = nh.subscribe("/map", 1, &OtonomBeyin::mapCallback, this);
        cmd_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
        unchanged_time = ros::Time::now();
        ROS_INFO("Otonom Beyin: Harita izleniyor...");
    }

    void mapCallback(const nav_msgs::OccupancyGrid::ConstPtr& msg) {
        if (is_done) return;
        int current_area = 0;
        for (size_t i = 0; i < msg->data.size(); ++i) {
            if (msg->data[i] != -1) current_area++;
        }

        if (current_area > last_area + 100) {
            last_area = current_area;
            unchanged_time = ros::Time::now();
        } else {
            double gecen_sure = (ros::Time::now() - unchanged_time).toSec();
            ROS_INFO_THROTTLE(2.0, "Harita Alani: %d, Kesif Yok Suresi: %.1f / 45.0 sn", current_area, gecen_sure);

            if (gecen_sure > 45.0 && last_area > 4000) {
                is_done = true;
                gecisYap();
            }
        }
    }

    void gecisYap() {
        ROS_INFO("Haritalama tamamlandi. Kasif durduruluyor...");
        system("rosnode kill /kasif_node");
        ros::Duration(1.0).sleep();

        ROS_INFO("Robot duruyor");
        geometry_msgs::Twist stop_cmd;
        stop_cmd.linear.x = 0.0; stop_cmd.angular.z = 0.0;
        for(int i=0; i<5; i++){
            cmd_pub.publish(stop_cmd);
            ros::Duration(0.2).sleep();
        }

        ROS_INFO("Harita kaydediliyor...");
        system("rosrun map_server map_saver -f $HOME/benim_haritam");
        ros::Duration(2.0).sleep();

        ROS_INFO("Harita 'benim_haritam' adiyla ana dizine kaydedildi. Asama 2'ye (Navigasyon) gecebilirsiniz.");
        ros::shutdown();
    }
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "otonom_beyin_node");
    OtonomBeyin beyin;
    ros::spin();
    return 0;
}
