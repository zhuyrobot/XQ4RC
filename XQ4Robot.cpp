﻿#include <cscv/base/acxx.h>
#include <ams_xq/msg/xq4_frame.hpp>
#include <ams_xq/srv/set_string.hpp>
#include "XQ4IO.h"

class XQ4ROS2Server : public rclcpp::Node
{
public://XQ4Node
	XQ4IO xq4io;
	XQ4ROS2Server(string port = "", string nn = "XQ4Server", string ns = "XQ") : Node(nn, ns) 
	{ 
		if (port.empty()) return;
		if (xq4io.open(port)) spdlog::info("Open {} failed", port);
	}

public://ManuPort
	rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srvClosePort = create_service<std_srvs::srv::Trigger>("ClosePort",
		[this](const std_srvs::srv::Trigger::Request::SharedPtr req, std_srvs::srv::Trigger::Response::SharedPtr res)->void
		{
			res->message = "Close " + xq4io.name() + " succeeded";
			if (xq4io.opened()) xq4io.close();
			else res->message = xq4io.name() + " not opened";
		});

	//ros2 service call /XQ/OpenPort ams_xq/srv/SetString "{str: 'COM2'}"
	rclcpp::Service<ams_xq::srv::SetString>::SharedPtr srvManuPort = create_service<ams_xq::srv::SetString>("OpenPort",
		[this](const ams_xq::srv::SetString::Request::SharedPtr req, ams_xq::srv::SetString::Response::SharedPtr res)->void
		{
			if (xq4io.opened()) res->msg = xq4io.name() + " not closed";
			else res->msg = "Open " + xq4io.name() + (xq4io.open(req->str) ? " succeeded" : " failed and check whether to set right port name");
		});

public://ManuCar
	rclcpp::Service<ams_xq::srv::SetString>::SharedPtr srvSetMode = create_service<ams_xq::srv::SetString>("SetMode",
		[this](const ams_xq::srv::SetString::Request::SharedPtr req, ams_xq::srv::SetString::Response::SharedPtr res)->void
		{
			res->msg = "Done";
			xq4io.setMode(req->str[0]);
		});

	//ros2 service call /XQ/OpenPort ams_xq/srv/SetString "{str: '1'}" //0/1/2
	rclcpp::Service<ams_xq::srv::SetString>::SharedPtr srvRunSensor = create_service<ams_xq::srv::SetString>("RunSensor",
		[this](const ams_xq::srv::SetString::Request::SharedPtr req, ams_xq::srv::SetString::Response::SharedPtr res)->void
		{
			res->msg = "Done";
			xq4io.runSensor(int(req->str[0]) - 48);
		});

	//ros2 service call -r 1 /XQ/RunMotor ams_xq/srv/SetString "{str: 'FF22'}" //forward50 //SS22 for brake50
	rclcpp::Service<ams_xq::srv::SetString>::SharedPtr srvRunMotor = create_service<ams_xq::srv::SetString>("RunMotor",
		[this](const ams_xq::srv::SetString::Request::SharedPtr req, ams_xq::srv::SetString::Response::SharedPtr res)->void
		{
			res->msg = "Done";
			if (req->str.size() > 3) xq4io.runMotor(req->str[0], req->str[1], req->str[2], req->str[3]);
			else res->msg = "Failed (too few params)";
		});

	//ros2 topic pub -r 1 /XQ/RunCar std_msgs/msg/String "{data: 'f2'}" //forward50 //s2 for brake50
	rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subRunCar = create_subscription<std_msgs::msg::String>("RunCar", 10,
		[&](const std_msgs::msg::String::SharedPtr val)->void 
		{ 
			if(val->data.size() > 1) xq4io.runCar(val->data[0], val->data[1]); 
			else spdlog::error("Published data not enough");
		});

public://Update
	rclcpp::Publisher<ams_xq::msg::XQ4Frame>::SharedPtr pubXQFrame = create_publisher<ams_xq::msg::XQ4Frame>("XQ4Frame", 10);
    rclcpp::TimerBase::SharedPtr TimerXQFrame = create_wall_timer(50ms, [&]()->void
        {
			if(!xq4io.opened()) { this_thread::sleep_for(2000ms); spdlog::error("No port opened"); return; }
			XQ4IO::XQ4Frame *frame = 0;
            xq4io.getStatus(&frame);
			ams_xq::msg::XQ4Frame rosFrame;
			if(frame) memcpy(&rosFrame, frame, sizeof(rosFrame));
			else spdlog::warn("Invalid frame and it is possible to connect one wrong port if this happens continuously");
            pubXQFrame->publish(rosFrame);
        });

public:
	static void RunMe(int argc = 0, char** argv = 0) 
	{ 
		rclcpp::init(argc, argv);

		auto server = std::make_shared<XQ4ROS2Server>();
		vector<string> portnames;
		for (int k = 0; k < 100; ++k) portnames.push_back("/dev/ttyUSB" + std::to_string(k));
		for (int k = 0; k < 100; ++k) portnames.push_back("COM" + std::to_string(k));
		for (int k = 0; k < portnames.size(); ++k)
			if (server->xq4io.open(portnames[k])) break;
		if(!server->xq4io.opened()) { spdlog::error("Open port failed"); }
		else spdlog::info("{} has opened successfully", server->xq4io.name());
		rclcpp::spin(server);
		
		rclcpp::shutdown();	
	}
};

int main(int argc, char** argv) { XQ4ROS2Server::RunMe(argc, argv); return 0; }