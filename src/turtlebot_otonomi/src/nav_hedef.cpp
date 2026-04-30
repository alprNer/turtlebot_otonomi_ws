#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <geometry_msgs/Twist.h>
#include <std_srvs/Empty.h> 

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

struct Waypoint { double x; double y; double w; };

void etrafiTaraVeKonumlan(ros::NodeHandle& nh) {
    while(ros::ok() && ros::Time::now().toSec() == 0.0) {
        ros::Duration(0.1).sleep();
        ros::spinOnce();
    }

    ROS_INFO("AMCL Local Convergence: Lidar ile baslangic konumu hassaslastiriliyor...");
    ros::Publisher cmd_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Rate rate(10);
    geometry_msgs::Twist spin_cmd;
    spin_cmd.angular.z = 0.5; 

    ros::Time baslangic = ros::Time::now();
    while (ros::ok() && (ros::Time::now() - baslangic).toSec() < 25.0) {
        cmd_pub.publish(spin_cmd);
        ros::spinOnce();
        rate.sleep();
    }

    spin_cmd.angular.z = 0.0;
    for(int i=0; i<3; i++) { cmd_pub.publish(spin_cmd); ros::Duration(0.1).sleep(); }
    ros::Duration(2.0).sleep(); 
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "nav_hedef_node");
    ros::NodeHandle nh;

    etrafiTaraVeKonumlan(nh);

    MoveBaseClient ac("move_base", true);
    ROS_INFO("move_base action server bekleniyor...");
    ac.waitForServer();
    ROS_INFO("Server baglantisi basarili.");

    
    ros::ServiceClient clear_costmaps_client = nh.serviceClient<std_srvs::Empty>("/move_base/clear_costmaps");
    std_srvs::Empty clear_srv;

    Waypoint points[5] = {
    {0.5, 0.5, 1.0},    
    {1.2, -0.5, 1.0},   
    {0.5, -1.2, 1.0},   
    {-0.5, -0.5, 1.0},  
    {-1.5, 0.8, 1.0}    
    };

    for(int i = 0; i < 5; i++) {
       
        ROS_INFO("Hedef %d oncesi hafiza (Local Costmap) temizleniyor...", i+1);
        clear_costmaps_client.call(clear_srv);
        ros::Duration(1.0).sleep();

        move_base_msgs::MoveBaseGoal goal;
        goal.target_pose.header.frame_id = "map";
        goal.target_pose.header.stamp = ros::Time::now();
        goal.target_pose.pose.position.x = points[i].x;
        goal.target_pose.pose.position.y = points[i].y;
        goal.target_pose.pose.orientation.w = points[i].w;

        ROS_INFO("Hedef %d gonderiliyor: X=%.2f, Y=%.2f", i+1, points[i].x, points[i].y);
        ac.sendGoal(goal);
        ac.waitForResult();

        if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
            ROS_INFO("Basarili! Robot Hedef %d noktasina ulasti.", i+1);
        else
            ROS_INFO("Basarisiz! Robot Hedef %d noktasina ulasamadi.", i+1);
            
        ros::Duration(2.0).sleep(); 
    }

    ROS_INFO("Tum hedeflere ulasildi. Senaryo tamamlandi.");
    return 0;
}