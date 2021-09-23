#include "vhdl_templates.h"
#include <string>

void vhdl_templates::vhdl_pulser(std::stringstream &ss){
	ss<<pulser_template;
}

void vhdl_templates::vhdl_ffd(std::stringstream &ss){
	ss<<ffd_template;
}

void vhdl_templates::vhdl_entity(std::stringstream &ss,std::string entity_name,std::vector<std::string> generics,std::vector<std::string> ports,std::string arch_name,std::string signals,std::string decls){

	std::string g;
	if(generics.size()!=0){
		g=generics.front();
		for(auto it=generics.begin()+1;it!=generics.end();it++){
			g+=";\n"+*it;
		}
	}

	std::string p;
	if(ports.size()!=0){
		p=ports.front();
		for(auto it=ports.begin()+1;it!=ports.end();it++){
			p+=";\n"+*it;
		}
	}

	ss<<boost::format(entity_template)
		%entity_name
		%g
		%p
		%arch_name
		%signals
		%decls
		<<std::endl;
}

void vhdl_templates::vhdl_entity_inst(std::stringstream &ss,std::string inst_name,std::string entity_name,std::vector<std::string> generic_map,std::vector<std::string> port_map){

	std::string gm;
	if(generic_map.size()!=0){
		gm=generic_map.front();
		for(auto it=generic_map.begin()+1;it!=generic_map.end();it++){
			gm+=","+*it;
		}
	}

	std::string pm;
	if(port_map.size()!=0){
		pm=port_map.front();
		for(auto it=port_map.begin()+1;it!=port_map.end();it++){
			pm+=","+*it;
		}
	}

	ss<<boost::format(entity_inst_template)
		%inst_name
		%entity_name
		%gm
		%pm
		<<std::endl;
}

std::string vhdl_templates::vhdl_entity_generic(std::string gen_name,std::string gen_type){
	return (boost::format(entity_generic)
			%gen_name
			%gen_type).str();
}

std::string vhdl_templates::vhdl_entity_port(std::string port_name,std::string inout,unsigned int dim){
	return (boost::format(entity_port)
			%port_name
			%inout
			%(dim-1)).str();
}

std::string vhdl_templates::vhdl_mapping(std::string block_v,std::string our_v){
	return (boost::format(inst_mapping_template)
		%block_v
		%our_v).str();	
}

void vhdl_templates::vhdl_signal(std::stringstream &ss_decl,std::string name,unsigned int dim){
	ss_decl<<boost::format(signal_decl)
		%name
		%(dim-1)
		<<std::endl;
}

void vhdl_templates::vhdl_assign(std::stringstream &ss_decl,std::string dest,std::string src){
	ss_decl<<boost::format(signal_assign)
		%dest
		%src
		<<std::endl;
}

void vhdl_templates::vhdl_resize(std::stringstream &ss_decl,std::string dest,std::string src,std::string type,unsigned int dim){
	ss_decl<<boost::format(signal_resize)
		%dest
		%type
		%src
		%dim
		<<std::endl;
}

void vhdl_templates::vhdl_add(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name){
	ss_statm<<boost::format(add_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_sub(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name){
	ss_statm<<boost::format(sub_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_sim(std::stringstream &ss_statm,std::string name,std::string type,std::string target_name){
	ss_statm<<boost::format(sim_template)
		%name
		%type
		%target_name
		<<std::endl;
}

void vhdl_templates::vhdl_mult(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name,unsigned int dim){
	ss_statm<<boost::format(mult_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		%dim
		<<std::endl;
}

void vhdl_templates::vhdl_div(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name){
	ss_statm<<boost::format(div_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_mod(std::stringstream &ss_statm,std::string name,std::string left_type,std::string right_type,std::string left_name,std::string right_name){
	ss_statm<<boost::format(mod_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_shl(std::stringstream &ss_statm,std::string name,std::string type,std::string toshift_name,std::string amnt_name){ 
	ss_statm<<boost::format(shl_template)
		%name
		%type
		%toshift_name
		%amnt_name
		<<std::endl;
}

void vhdl_templates::vhdl_shr(std::stringstream &ss_statm,std::string name,std::string type,std::string toshift_name,std::string amnt_name){
	ss_statm<<boost::format(shr_template)
		%name
		%type
		%toshift_name
		%amnt_name
		<<std::endl;
}

void vhdl_templates::vhdl_rtl(std::stringstream &ss_statm,std::string name,std::string type,std::string torotate_name,std::string amnt_name){
	ss_statm<<boost::format(rtl_template)
		%name
		%type
		%torotate_name
		%amnt_name
		<<std::endl;
}

void vhdl_templates::vhdl_rtr(std::stringstream &ss_statm,std::string name,std::string type,std::string torotate_name,std::string amnt_name){
	ss_statm<<boost::format(rtr_template)
		%name
		%type
		%torotate_name
		%amnt_name
		<<std::endl;
}

void vhdl_templates::vhdl_and(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name){
	ss_statm<<boost::format(and_template)
		%name
		%left_name
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_or(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name){
	ss_statm<<boost::format(or_template)
		%name
		%left_name
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_xor(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name){
	ss_statm<<boost::format(xor_template)
		%name
		%left_name
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_not(std::stringstream &ss_statm,std::string name,std::string target_name){
	ss_statm<<boost::format(not_template)
		%name
		%target_name
		<<std::endl;
}

void vhdl_templates::vhdl_equal(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name){ 
	ss_statm<<boost::format(equal_template)
		%name
		%left_name
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_diff(std::stringstream &ss_statm,std::string name,std::string left_name,std::string right_name){ 
	ss_statm<<boost::format(diff_template)
		%name
		%left_name
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_more(std::stringstream &ss_statm,std::string name,std::string left_type,std::string left_name,std::string right_type,std::string right_name){ 
	ss_statm<<boost::format(more_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_less(std::stringstream &ss_statm,std::string name,std::string left_type,std::string left_name,std::string right_type,std::string right_name){ 
	ss_statm<<boost::format(less_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_moreq(std::stringstream &ss_statm,std::string name,std::string left_type,std::string left_name,std::string right_type,std::string right_name){ 
	ss_statm<<boost::format(moreq_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_leseq(std::stringstream &ss_statm,std::string name,std::string left_type,std::string left_name,std::string right_type,std::string right_name){ 
	ss_statm<<boost::format(leseq_template)
		%name
		%left_type
		%left_name
		%right_type
		%right_name
		<<std::endl;
}

void vhdl_templates::vhdl_const(std::stringstream &ss_decl,std::string name,unsigned int dim,boost::dynamic_bitset<> value){
	std::string num;
	boost::to_string(value,num);

	ss_decl<<boost::format(const_template)
		%name
		%(dim-1)
		%num
		<<std::endl;
}

//////

std::string vhdl_templates::pulser_template=R"(
--Pre fabricated module
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

entity pulser is
    generic(    CLK_FREQ : real;
                TICK_FREQ : real);
    port(   clk : in std_logic;
            tick : out std_logic_vector(0 downto 0));
end pulser;

architecture behavioural of pulser is
    constant MAX_CNT : positive := positive(CLK_FREQ/TICK_FREQ);
    constant N_BITS : positive := positive(ceil(log2(real(MAX_CNT))));
    signal s_cnt : unsigned(N_BITS-1 downto 0);
begin
    process(clk)
    begin
        if rising_edge(clk) then
            s_cnt<=s_cnt+1;
            if s_cnt=MAX_CNT-1 then
                s_cnt<=(others=>'0');
            end if;
        end if;
    end process;
    tick<="1" when s_cnt=MAX_CNT-1 else "0";
end behavioural;
)";

std::string vhdl_templates::ffd_template=R"(
--Pre fabricated module
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

entity ffd is
    generic(SIZE : positive);
    port(clk : in std_logic;
			en : in std_logic_vector(0 downto 0);
			rst : in std_logic;
			rst_val : in std_logic_vector(SIZE-1 downto 0);
			din : in std_logic_vector(SIZE-1 downto 0);
			dout : out std_logic_vector(SIZE-1 downto 0));
end ffd;

architecture behavioural of ffd is
    signal s_data : std_logic_vector(SIZE-1 downto 0);
begin
	process(clk)
	begin
		if(rising_edge(clk)) then
			if(rst='1') then
				s_data<=rst_val;
			elsif(en="1") then
				s_data<=din;
			end if;
		end if;
	end process;
	dout<=s_data;
end behavioural;		
)";

std::string vhdl_templates::entity_template=R"(
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity %1% is
    generic(%2%);
    port(%3%);
end %1%;

architecture %4% of %1% is
    %5%
begin
    %6%
end %4%;
)";

std::string vhdl_templates::entity_inst_template=R"(
%1% : entity work.%2%
    generic map(%3%)
    port map(%4%);
)";

std::string vhdl_templates::entity_generic="%1% : %2%";
std::string vhdl_templates::entity_port="%1% : %2% std_logic_vector(%3% downto 0)";

std::string vhdl_templates::inst_mapping_template="%1%=>%2%";

std::string vhdl_templates::signal_decl="signal %1% : std_logic_vector(%2% downto 0);";
std::string vhdl_templates::signal_assign="%1% <= %2%;";
std::string vhdl_templates::signal_resize="%1% <= std_logic_vector(resize(%2%(%3%),%4%));";

std::string vhdl_templates::add_template="%1% <= std_logic_vector(%2%(%3%) + %4%(%5%));";
std::string vhdl_templates::sub_template="%1% <= std_logic_vector(%2%(%3%) - %4%(%5%));";
std::string vhdl_templates::sim_template="%1% <= std_logic_vector(-%2%(%3%));";
std::string vhdl_templates::mult_template="%1% <= std_logic_vector(resize(%2%(%3%) * %4%(%5%),%6%));";
std::string vhdl_templates::div_template="%1% <= std_logic_vector(%2%(%3%) / %4%(%5%));";
std::string vhdl_templates::mod_template="%1% <= std_logic_vector(%2%(%3%) rem %4%(%5%));";

std::string vhdl_templates::shl_template="%1% <= std_logic_vector(shift_left(%2%(%3%),to_integer(unsigned(%4%))));";
std::string vhdl_templates::shr_template="%1% <= std_logic_vector(shift_right(%2%(%3%),to_integer(unsigned(%4%))));";
std::string vhdl_templates::rtl_template="%1% <= std_logic_vector(rotate_left(%2%(%3%),to_integer(unsigned(%4%))));";
std::string vhdl_templates::rtr_template="%1% <= std_logic_vector(rotate_right(%2%(%3%),to_integer(unsigned(%4%))));";

std::string vhdl_templates::and_template="%1% <= %2% and %3%;";
std::string vhdl_templates::or_template="%1% <= %2% or %3%;";
std::string vhdl_templates::xor_template="%1% <= %2% xor %3%;";
std::string vhdl_templates::not_template="%1% <= not %2%;";

std::string vhdl_templates::equal_template="%1% <= (0=>'1',others=>'0') when(%2% = %3%) else (others=>'0');"; 
std::string vhdl_templates::diff_template="%1% <= (0=>'1',others=>'0') when(%2% /= %3%) else (others=>'0');"; 
std::string vhdl_templates::more_template="%1% <= (0=>'1',others=>'0') when(%2%(%3%) > %4%(%5%)) else (others=>'0');"; 
std::string vhdl_templates::less_template="%1% <= (0=>'1',others=>'0') when(%2%(%3%) < %4%(%5%)) else (others=>'0');"; 
std::string vhdl_templates::moreq_template="%1% <= (0=>'1',others=>'0') when(%2%(%3%) => %4%(%5%)) else (others=>'0');"; 
std::string vhdl_templates::leseq_template="%1% <= (0=>'1',others=>'0') when(%2%(%3%) <= %4%(%5%)) else (others=>'0');"; 

std::string vhdl_templates::const_template="constant %1% : std_logic_vector(%2% downto 0) := \"%3%\";";
