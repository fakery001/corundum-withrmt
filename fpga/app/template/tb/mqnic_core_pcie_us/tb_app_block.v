`timescale 1ns / 1ps

module tb_app_block;

    // 参数定义
    parameter AXIS_DATA_WIDTH = 32;  // 数据宽度
    parameter TSTRB_WIDTH = AXIS_DATA_WIDTH / 8;  // 每字节的信号宽度
    parameter M_AXI_awlen = 16;
    
    // 信号声明
    reg clk;                      // 时钟信号
    reg rst;                      // 复位信号
    wire M_AXIS_TREADY;           // 目标端 ready 信号
    reg [AXIS_DATA_WIDTH-1 : 0] M_AXIS_TDATA;  // 数据信号
    reg [TSTRB_WIDTH-1 : 0] M_AXIS_TSTRB;      // byte qualifier
    reg M_AXIS_TLAST;            // TLAST 信号，指示数据包的边界
    reg M_AXIS_TVALID;           // TVALID 信号，表示数据有效
    reg M_AXIS_TUSER;            // TUSER 信号，指示帧的开始
    //clk signal
    localparam CYCLE = 10;
    assign M_AXIS_TREADY = 1'b1;
    // 模拟时钟
    always begin
        #5 clk = ~clk;  // 时钟周期 10ns (100MHz)
    end
    
    // DUT 实例化
    mqnic_app_block uut(
        .clk            (clk),
        .rst            (rst),
        .S_AXIS_TREADY  (M_AXIS_TREADY),
        .S_AXIS_TDATA   (M_AXIS_TDATA),
        .S_AXIS_TSTRB   (M_AXIS_TSTRB),
        .S_AXIS_TLAST   (M_AXIS_TLAST),
        .S_AXIS_TVALID  (M_AXIS_TVALID),
        .S_AXIS_TUSER    (M_AXIS_TUSER)
    );


    // 测试过程
    initial begin
        // 初始化信号
        clk = 0;
        rst = 1;
        //M_AXIS_TREADY = 1;    // 初始 TREADY 为 0
        M_AXIS_TVALID = 0;    // 初始 TVALID 为 0
        M_AXIS_TDATA = 0;
        M_AXIS_TSTRB = 0;
        M_AXIS_TLAST = 0;
        M_AXIS_TUSER = 0;
        
        // 复位测试
        $display("Simulation start...");
        $display("Applying reset...");
        #10 rst = 0;  // 10ns 后解除复位


        
        // 激活数据传输
        #20;  // 等待一些时间让系统稳定
            M_AXIS_TVALID <= 1;  // 激活数据有效信号
            M_AXIS_TDATA <= 32'h00000001;  // 数据
            M_AXIS_TSTRB <= 4'b1111;  // 字节使能
            M_AXIS_TLAST <= 0;  // 不是最后一帧
            M_AXIS_TUSER <= 1;  // 数据帧起始

        
        // #200;  // 等待一些时间让系统稳定
        repeat(M_AXI_awlen * 10) begin
            #(CYCLE * 2)
            M_AXIS_TDATA = M_AXIS_TDATA + 1'b1;
        end

        //     M_AXIS_TVALID <= 1;  // 激活数据有效信号
        //     M_AXIS_TDATA <= 32'hb5b5b5b5;  // 数据
        //     M_AXIS_TSTRB <= 4'b1111;  // 字节使能
        //     M_AXIS_TLAST <= 0;  // 不是最后一帧
        //     M_AXIS_TUSER <= 1;  // 数据帧起始
        // #200;  // 等待一些时间让系统稳定
        //     M_AXIS_TVALID <= 1;  // 激活数据有效信号
        //     M_AXIS_TDATA <= 32'hc5c5c5c5;  // 数据
        //     M_AXIS_TSTRB <= 4'b1111;  // 字节使能
        //     M_AXIS_TLAST <= 0;  // 不是最后一帧
        //     M_AXIS_TUSER <= 1;  // 数据帧起始
        // // 在每个时钟周期产生数据
        $display("Start driving data...");
    end

    // // 生成 AXI 信号
    // always @(posedge clk or posedge rst) begin
    //     if (rst) begin
    //         M_AXIS_TVALID <= 0;
    //         M_AXIS_TDATA <= 0;
    //         M_AXIS_TSTRB <= 0;
    //         M_AXIS_TLAST <= 0;
    //         M_AXIS_TUSER <= 0;
    //     end else begin
    //         // 激活 TVALID 和产生数据
    //         M_AXIS_TVALID <= 1;  // 激活数据有效信号
    //         M_AXIS_TDATA <= 32'hA5A5A5A5;  // 数据
    //         M_AXIS_TSTRB <= 4'b1111;  // 字节使能
    //         M_AXIS_TLAST <= 0;  // 不是最后一帧
    //         M_AXIS_TUSER <= 1;  // 数据帧起始
    //     end
    // end

    // 停止仿真
    initial begin
        #1000
        $finish;
    end

    // Monitor 接收端的 TREADY 信号，确保 AXI 传输的正确性
    initial begin
        $monitor("At time %t: TVALID=%b, TREADY=%b, TDATA=%h, TSTRB=%b, TLAST=%b, TUSER=%b", 
                  $time, uut.M_AXIS_TVALID, M_AXIS_TREADY, M_AXIS_TDATA, M_AXIS_TSTRB, M_AXIS_TLAST, M_AXIS_TUSER);
    end

endmodule