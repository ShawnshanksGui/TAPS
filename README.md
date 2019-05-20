# TAPS



一. 代码流程
代码分两部分:controller和server;


controller:
在一个时间每个周期内：controller执行四部分工作：
1）产生任务（待实现），执行任务调度算法；
2）建立一个子线程，监听来自各个server的 请求，每接受一个请求，建立一个子线程（子线程的子线程）；
3）在子线程（子线程的子线程）中，接收任务的调度请求（schedule of task），并进行回复；
4）收集各个task的完成情况（或者有效吞吐），待实现。



server（暂时由于两台servers， 0，1组成）:
在一个时间周期内：每台server执行以下三部分工作：
0）维持一个子线程，充当client，被动监听和接受来自server发起的连接（flow），每监听到一条流，建立一个thread（子线程的子线程）；

在主线程中，
1）server首先向controller请求任务和调度结果；
2）一个task由多条flow组成。 server根据每条流的五元组（server_ip. server_port, protocol number, client_ip, client_ip）， 按照调度结果（流起始时间）与clients分别建立连接（一个连接对应一个子线程）；在每条连接内，server发送的数据量 = flow size， 持续时间 = duration_time；
3）返回性能指标：在deadline时间内，有多少任务完成了（或者有效吞吐），待实现。



二. How To Run:
1）三台服务器，分别对应controller, server_0, server_1;
\n
\n
2）编译， 进入路径 ~/TAPS/src_test, 在三台服务器上分别运行：
	sh compile_controller.sh  
	sh compile_server_0.sh
	sh compile_server_1.sh 
\n
\n
3）执行，依次运行controller, server_0, server_1：
	./test_controller 
	./test_server_0  
	./test_server_1 
