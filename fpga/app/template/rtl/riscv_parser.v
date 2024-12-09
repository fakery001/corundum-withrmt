`timescale 1ns / 1ps

`define ETH_TYPE_IPV4	16'h0008
`define IPPROT_UDP		8'h11
`define CONTROL_PORT    16'hf2f1
`define RISCV_PORT      16'heeee

module riscv_parser #(
	parameter C_S_AXIS_DATA_WIDTH = 512,
	parameter C_S_AXIS_TUSER_WIDTH = 128
)
(
	input				clk,
	input				aresetn,

	// input Slave AXI Stream
	input [C_S_AXIS_DATA_WIDTH-1:0]			s_axis_tdata,
	input [((C_S_AXIS_DATA_WIDTH/8))-1:0]	s_axis_tkeep,
	input [C_S_AXIS_TUSER_WIDTH-1:0]		s_axis_tuser,
	input									s_axis_tvalid,
	output									s_axis_tready,
	input									s_axis_tlast,

	// output Master AXI Stream
	output reg [C_S_AXIS_DATA_WIDTH-1:0]		m_axis_tdata,
	output reg [((C_S_AXIS_DATA_WIDTH/8))-1:0]	m_axis_tkeep,
	output reg [C_S_AXIS_TUSER_WIDTH-1:0]		m_axis_tuser,
	output reg									m_axis_tvalid,
	input										m_axis_tready,
	output reg									m_axis_tlast,

	// riscv output
	output reg [C_S_AXIS_DATA_WIDTH-1:0]		c_m_axis_tdata,
	output reg [((C_S_AXIS_DATA_WIDTH/8))-1:0]	c_m_axis_tkeep,
	output reg [C_S_AXIS_TUSER_WIDTH-1:0]		c_m_axis_tuser,
	output reg									c_m_axis_tvalid,
	input										c_m_axis_tready,
	output reg									c_m_axis_tlast

);

wire [C_S_AXIS_DATA_WIDTH-1:0]	pkt_fifo_tdata;
wire [C_S_AXIS_DATA_WIDTH/8-1:0]	pkt_fifo_tkeep;
wire [C_S_AXIS_TUSER_WIDTH-1:0]		pkt_fifo_tuser;
wire								pkt_fifo_tlast;
reg								pkt_fifo_rd_en;
wire	pkt_fifo_empty, pkt_fifo_full;

//
assign s_axis_tready = !pkt_fifo_full;

fallthrough_small_fifo #(
	.WIDTH(C_S_AXIS_DATA_WIDTH+C_S_AXIS_TUSER_WIDTH+C_S_AXIS_DATA_WIDTH/8+1),
	.MAX_DEPTH_BITS(5)
)
seg_fifo (
	.din					({s_axis_tdata, s_axis_tkeep, s_axis_tuser, s_axis_tlast}),
	.wr_en					(s_axis_tvalid),
	//
	.rd_en					(pkt_fifo_rd_en),
	.dout					({pkt_fifo_tdata, pkt_fifo_tkeep, pkt_fifo_tuser, pkt_fifo_tlast}),
	//
	.full					(),
	.prog_full				(),
	.nearly_full			(pkt_fifo_full),
	.empty					(pkt_fifo_empty),
	.reset					(~aresetn),
	.clk					(clk)
);

localparam WAIT_FIRST_PKT=0, 
		   DROP_PKT=1, 
		   FLUSH_DATA=2,
		   FLUSH_CTL = 3;


reg [C_S_AXIS_DATA_WIDTH-1:0]		r_tdata;
reg [((C_S_AXIS_DATA_WIDTH/8))-1:0]	r_tkeep;
reg [C_S_AXIS_TUSER_WIDTH-1:0]		r_tuser;
reg									r_tvalid;
reg									r_tlast;

reg									r_s_tready;

reg [1:0] state, state_next;
//1 for control, 0 for data;
reg 								c_switch;
wire								w_c_switch;








assign w_c_switch = c_switch;




always @(*) begin

	r_tdata = pkt_fifo_tdata;
	r_tkeep = pkt_fifo_tkeep;
	r_tuser = pkt_fifo_tuser;
	r_tlast = pkt_fifo_tlast;

	r_tvalid = 0;
	// r_s_tready = m_axis_tready;

	c_switch = 1'b0;

	pkt_fifo_rd_en = 0;

	state_next = state;



	case (state) 
		WAIT_FIRST_PKT: begin
			if (!pkt_fifo_empty) begin
				if (m_axis_tready) begin
					if (1) begin
						//checkme: we put the security check here
						// if(s_axis_tdata[335:320] == `CONTROL_PORT && cookie_w == cookie_val_d1) begin
						if(pkt_fifo_tdata[335:320] == `RISCV_PORT) begin
							state_next = FLUSH_CTL;
							c_switch = 1'b1;

							pkt_fifo_rd_en = 1;
							r_tvalid = 1;
						end
						else if (!pkt_fifo_tlast) begin
							//checkme: if this vlan is not configed, send it
							// TODO: for validity check
							//if((vlan_id_one_hot_w & vlan_drop_flags)==0) begin
							if (1) begin
								pkt_fifo_rd_en = 1;
								r_tvalid = 1;
								state_next = FLUSH_DATA;
								c_switch = 1'b0;

							end
							else begin
								pkt_fifo_rd_en = 1;
								state_next = DROP_PKT;
								r_tvalid = 0;
							end
						end

						else if (pkt_fifo_tlast) begin
							pkt_fifo_rd_en = 1;
							state_next = WAIT_FIRST_PKT;
							c_switch = 1'b0;

							r_tvalid = 1;
							//checkme: if this vlan is configed, drop it
							//if((vlan_id_one_hot_w & vlan_drop_flags)!=0) begin
							//	r_tvalid = 0;
							//end
						end
					end
					else begin
						
						pkt_fifo_rd_en = 1;
						r_tvalid = 0;
						state_next = DROP_PKT;
					end

					if (pkt_fifo_tlast) begin
						state_next = WAIT_FIRST_PKT;
					end
				end

				else begin
					c_switch = 1'b0;
				end
			end
		end
		FLUSH_DATA: begin
			if (!pkt_fifo_empty) begin
				if (m_axis_tready) begin
					r_tvalid = 1;
					pkt_fifo_rd_en = 1;
					if (pkt_fifo_tlast) begin
						state_next = WAIT_FIRST_PKT;
					end
				end
			end
		end
		FLUSH_CTL: begin
			c_switch = 1'b1;
			if (!pkt_fifo_empty) begin
				r_tvalid = 1;
				pkt_fifo_rd_en = 1;
				if (pkt_fifo_tlast) begin
					state_next = WAIT_FIRST_PKT;
				end
			end
		end
		DROP_PKT: begin
			r_tvalid = 0;
			if (!pkt_fifo_empty) begin
				pkt_fifo_rd_en = 1;
				if (pkt_fifo_tlast) begin
					state_next = WAIT_FIRST_PKT;
				end
			end
		end
	endcase
end

always @(posedge clk or negedge aresetn) begin
	if (~aresetn) begin
		state <= WAIT_FIRST_PKT;

		m_axis_tdata <= 0;
		m_axis_tkeep <= 0;
		m_axis_tuser <= 0;
		m_axis_tlast <= 0;

		m_axis_tvalid <= 0;

		c_m_axis_tdata <= 0;
		c_m_axis_tkeep <= 0;
		c_m_axis_tuser <= 0;
		c_m_axis_tlast <= 0;

		c_m_axis_tvalid <= 0;
		// s_axis_tready <= 0;

		//



	end

	else begin
		state <= state_next;


		if(!w_c_switch) begin // data pkt
			m_axis_tdata <= r_tdata;
			m_axis_tkeep <= r_tkeep;
			m_axis_tuser <= r_tuser;
			m_axis_tlast <= r_tlast;
			m_axis_tvalid <= r_tvalid;

			// s_axis_tready <= r_s_tready;
			//reset control path output 
			c_m_axis_tdata <= 0;
			c_m_axis_tkeep <= 0;
			c_m_axis_tuser <= 0;
			c_m_axis_tlast <= 0;
			c_m_axis_tvalid <= 0;

		end
		else begin // ctrl pkt
			m_axis_tdata <= 0;
			m_axis_tkeep <= 0;
			m_axis_tuser <= 0;
			m_axis_tlast <= 0;
			m_axis_tvalid <= 0;
			// 
			c_m_axis_tdata <= r_tdata;
			c_m_axis_tkeep <= r_tkeep;
			c_m_axis_tuser <= r_tuser;
			c_m_axis_tlast <= r_tlast;

			c_m_axis_tvalid <= r_tvalid;
		end
		
	end
end


endmodule
