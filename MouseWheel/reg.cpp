#include <freehdl/kernel.h>
#include <freehdl/std.h>


/* External declarations */
/* Definitions for enumeration type :ieee:std_logic_1164:std_ulogic */
class L4ieee_Q14std_logic_1164_I10std_ulogic:public enum_info_base{
   static const char *values[];
public:
   L4ieee_Q14std_logic_1164_I10std_ulogic():enum_info_base(0,8,values) {};
   static const char **get_values() { return values; }
   static int low() { return 0; }
   static int high() { return 8; }
   static int left() { return 0; }
   static int right() { return 8; }
};
extern L4ieee_Q14std_logic_1164_I10std_ulogic L4ieee_Q14std_logic_1164_I10std_ulogic_INFO;
/** Enumeration info class :ieee:std_logic_1164:std_logic */
class L4ieee_Q14std_logic_1164_I9std_logic:public enum_info_base{
   static const char **values;
public:
   L4ieee_Q14std_logic_1164_I9std_logic():enum_info_base(0,8,values) {};
   static const char **get_values() { return values; }
   static int low() { return 0; }
   static int high() { return 8; }
   static int left() { return 0; }
   static int right() { return 8; }
};
extern L4ieee_Q14std_logic_1164_I9std_logic L4ieee_Q14std_logic_1164_I9std_logic_INFO;
/* No header for IIR_EnumerationLiteral */
extern array_info L4ieee_Q14std_logic_1164_I16std_logic_vector_INFO;
#define L4ieee_Q14std_logic_1164_T16std_logic_vector array_type<enumeration >
#define L4ieee_Q14std_logic_1164_I16std_logic_vector array_info

/* End of external declarations */
#define L4work_E3reg_T3_t0 array_type<enumeration >
#define L4work_E3reg_I3_t0 array_info

#define L4work_E3reg_T3_t1 array_type<enumeration >
#define L4work_E3reg_I3_t1 array_info

/* Entity class declaration */
class L4work_E3reg {
public:
   void *father_component;
  L4work_E3reg (name_stack &iname, map_list *mlist, void *father);
  ~L4work_E3reg() {};
  integer L4work_E3reg_C7regsize,L4work_E3reg_C10startvalue;
  L4work_E3reg_I3_t0 L4work_E3reg_I3_t0_INFO;
  sig_info<L4work_E3reg_T3_t0> *L4work_E3reg_S5input;
  L4work_E3reg_I3_t1 L4work_E3reg_I3_t1_INFO;
  sig_info<L4work_E3reg_T3_t1> *L4work_E3reg_S6output;
  sig_info<enumeration> *L4work_E3reg_S3rst,*L4work_E3reg_S8writeclk;
};

/* Implementation of entity methods */
L4work_E3reg::
L4work_E3reg(name_stack &iname, map_list *mlist, void *father) {
    father_component = father;
    iname.push("");
    kernel.init_generic(&L4work_E3reg_C7regsize,(&L3std_Q8standard_I7integer_INFO),iname,":regsize",":work:reg",mlist,NULL,this);
    kernel.init_generic(&L4work_E3reg_C10startvalue,(&L3std_Q8standard_I7integer_INFO),iname,":startvalue",":work:reg",mlist,NULL,this);
    L4work_E3reg_I3_t0_INFO.set((&L4ieee_Q14std_logic_1164_I16std_logic_vector_INFO)->element_type,parray_info((&L4ieee_Q14std_logic_1164_I16std_logic_vector_INFO))->index_type,(L4work_E3reg_C7regsize-1),downto,0,-1).register_type(":work:reg",":work:reg:_t0",":_t0",NULL);
    L4work_E3reg_I3_t1_INFO.set((&L4ieee_Q14std_logic_1164_I16std_logic_vector_INFO)->element_type,parray_info((&L4ieee_Q14std_logic_1164_I16std_logic_vector_INFO))->index_type,(L4work_E3reg_C7regsize-1),downto,0,-1).register_type(":work:reg",":work:reg:_t1",":_t1",NULL);
    L4work_E3reg_S5input=new sig_info<L4work_E3reg_T3_t0>(iname,":input",":work:reg",mlist,(&L4work_E3reg_I3_t0_INFO),vIN,this);
    L4work_E3reg_S6output=new sig_info<L4work_E3reg_T3_t1>(iname,":output",":work:reg",mlist,(&L4work_E3reg_I3_t1_INFO),vOUT,this);
    L4work_E3reg_S6output->init(L4work_E3reg_T3_t1(new array_info((&L4work_E3reg_I3_t1_INFO)->element_type,(&L4work_E3reg_I3_t1_INFO)->index_type,(L4work_E3reg_C7regsize-1),downto,0,0),enumeration(2)));
    L4work_E3reg_S3rst=new sig_info<enumeration>(iname,":rst",":work:reg",mlist,(&L4ieee_Q14std_logic_1164_I9std_logic_INFO),vIN,this);
    L4work_E3reg_S8writeclk=new sig_info<enumeration>(iname,":writeclk",":work:reg",mlist,(&L4ieee_Q14std_logic_1164_I9std_logic_INFO),vIN,this);
    iname.pop();
};


/* Initialization function for entity :work:reg */
int L3std_Q8standard_init ();
int L4ieee_Q11numeric_std_init ();
int L4ieee_Q14std_logic_1164_init ();
bool L4work_E3reg_init_done = false;
int L4work_E3reg_init(){
if (L4work_E3reg_init_done) return 1;
L4work_E3reg_init_done=true;
L3std_Q8standard_init ();
L4ieee_Q11numeric_std_init ();
L4ieee_Q14std_logic_1164_init ();
register_source_file("/home/simo/My_Projects/Levitatore/MouseWheel/reg.vhdl","reg.vhdl");
name_stack iname;
iname.push("");
iname.pop();
return 1;
}

/* end of L4work_E3reg Entity */
/* External declarations */
/* No header for IIR_EnumerationLiteral */
extern array_info L4ieee_Q11numeric_std_I8unsigned_INFO;
#define L4ieee_Q11numeric_std_T8unsigned array_type<enumeration >
#define L4ieee_Q11numeric_std_I8unsigned array_info

/* Prototype for subprogram :ieee:numeric_std:to_unsigned */
L4ieee_Q11numeric_std_T8unsigned L4ieee_Q11numeric_std_Y11to_unsigned_i284(const integer ,const integer );
/* Prototype for predefined function :ieee:std_logic_1164:"=" */
/* End of external declarations */
#define L4work_E3reg_A7reg_beh_P10regprocess_T3_t2 array_type<enumeration >
#define L4work_E3reg_A7reg_beh_P10regprocess_I3_t2 array_info

/* Architecture class declaration */
class L4work_E3reg_A7reg_beh : public L4work_E3reg {
public:
  L4work_E3reg_A7reg_beh (name_stack &iname, map_list *mlist, void *father, int level);
  ~L4work_E3reg_A7reg_beh();
};

/* Concurrent statement class declaration(s) */


/* Class decl. process regprocess */
class L4work_E3reg_A7reg_beh_P10regprocess : public process_base {
public:
  L4work_E3reg_A7reg_beh_P10regprocess(L4work_E3reg_A7reg_beh* L4work_E3reg_A7reg_beh_AP_PAR,name_stack &iname);
  ~L4work_E3reg_A7reg_beh_P10regprocess() {};
  bool execute();
  L4work_E3reg_A7reg_beh* L4work_E3reg_A7reg_beh_AP;
  driver_info *L4work_E3reg_D6output;
  enumeration *L4work_E3reg_R3rst,*L4work_E3reg_R8writeclk;
  L4work_E3reg_T3_t0 *L4work_E3reg_R5input;
  sig_info<enumeration> *L4work_E3reg_S8writeclk;
  L4work_E3reg_A7reg_beh_P10regprocess_I3_t2 L4work_E3reg_A7reg_beh_P10regprocess_I3_t2_INFO;
  L4work_E3reg_A7reg_beh_P10regprocess_T3_t2 L4work_E3reg_A7reg_beh_P10regprocess_V3val;
  winfo_item winfo[1];
  L4work_E3reg_A7reg_beh *arch;
};
  /* Implementation of process :work:reg(reg_beh):regprocess methods */
L4work_E3reg_A7reg_beh_P10regprocess::
L4work_E3reg_A7reg_beh_P10regprocess(L4work_E3reg_A7reg_beh *L4work_E3reg_A7reg_beh_AP_PAR,name_stack &iname) : process_base(iname) {
    L4work_E3reg_A7reg_beh_AP=L4work_E3reg_A7reg_beh_AP_PAR;
    L4work_E3reg_S8writeclk=L4work_E3reg_A7reg_beh_AP->L4work_E3reg_S8writeclk;
    L4work_E3reg_R3rst=&L4work_E3reg_A7reg_beh_AP->L4work_E3reg_S3rst->reader();
    L4work_E3reg_R8writeclk=&L4work_E3reg_A7reg_beh_AP->L4work_E3reg_S8writeclk->reader();
    L4work_E3reg_R5input=&L4work_E3reg_A7reg_beh_AP->L4work_E3reg_S5input->reader();
    L4work_E3reg_A7reg_beh_P10regprocess_I3_t2_INFO.set((&L4ieee_Q14std_logic_1164_I16std_logic_vector_INFO)->element_type,parray_info((&L4ieee_Q14std_logic_1164_I16std_logic_vector_INFO))->index_type,(L4work_E3reg_A7reg_beh_AP->L4work_E3reg_C7regsize-1),downto,0,-1).register_type(":work:reg(reg_beh):regprocess",":work:reg(reg_beh):regprocess:_t2",":_t2",NULL);
    L4work_E3reg_A7reg_beh_P10regprocess_V3val.init((&L4work_E3reg_A7reg_beh_P10regprocess_I3_t2_INFO),enumeration(0));
    L4work_E3reg_A7reg_beh_P10regprocess_V3val=L4work_E3reg_A7reg_beh_P10regprocess_T3_t2(new array_info((&L4work_E3reg_A7reg_beh_P10regprocess_I3_t2_INFO)->element_type,(&L4work_E3reg_A7reg_beh_P10regprocess_I3_t2_INFO)->index_type,(L4work_E3reg_A7reg_beh_AP->L4work_E3reg_C7regsize-1),downto,0,0),enumeration(2));
    L4work_E3reg_D6output=kernel.get_driver(this,L4work_E3reg_A7reg_beh_AP->L4work_E3reg_S6output);
    {
     sigacl_list sal(3);
     sal.add(L4work_E3reg_A7reg_beh_AP->L4work_E3reg_S8writeclk);
     sal.add(L4work_E3reg_A7reg_beh_AP->L4work_E3reg_S5input);
     sal.add(L4work_E3reg_A7reg_beh_AP->L4work_E3reg_S3rst);
     winfo[0]=kernel.setup_wait_info(sal,this);
    }
}
bool L4work_E3reg_A7reg_beh_P10regprocess::execute() {
    if(((*L4work_E3reg_R3rst)==enumeration(2))) {
      L4work_E3reg_A7reg_beh_P10regprocess_V3val=array_alias<L4ieee_Q14std_logic_1164_T16std_logic_vector >((&L4ieee_Q14std_logic_1164_I16std_logic_vector_INFO),L4ieee_Q11numeric_std_Y11to_unsigned_i284 (L4work_E3reg_A7reg_beh_AP->L4work_E3reg_C10startvalue,L4work_E3reg_A7reg_beh_AP->L4work_E3reg_C7regsize));
      L4work_E3reg_D6output->inertial_assign(L4work_E3reg_A7reg_beh_P10regprocess_V3val,0,vtime(0));
    } else {
      if((attr_scalar_EVENT(L4work_E3reg_S8writeclk)&&((*L4work_E3reg_R8writeclk)==enumeration(3)))) {
        L4work_E3reg_A7reg_beh_P10regprocess_V3val=(*L4work_E3reg_R5input);
        L4work_E3reg_D6output->inertial_assign(L4work_E3reg_A7reg_beh_P10regprocess_V3val,0,vtime(0));
      }
    }
    return true;
}

/* handle for simulator to find architecture */
void*
L4work_E3reg_A7reg_beh_handle(name_stack &iname, map_list *mlist, void *father, int level) {
 REPORT(cout << "Starting constructor L4work_E3reg_A7reg_beh ..." << endl);
 return new L4work_E3reg_A7reg_beh(iname, mlist, father, level);
};
extern int L4work_E3reg_A7reg_beh_init ();
handle_info *L4work_E3reg_A7reg_beh_hinfo =
  add_handle("work","reg","reg_beh",&L4work_E3reg_A7reg_beh_handle,&L4work_E3reg_A7reg_beh_init);
/* Architecture Constructor */
L4work_E3reg_A7reg_beh::
L4work_E3reg_A7reg_beh(name_stack &iname, map_list *mlist, void *father, int level) :
  L4work_E3reg(iname, mlist, father) {
    iname.push(":reg_beh");
    iname.push("");
    kernel.add_process(new L4work_E3reg_A7reg_beh_P10regprocess(this,iname.set(":regprocess")),":work:reg(reg_beh)",":regprocess",this);
    iname.pop(); /* pop last declaration from name stack */ iname.pop(); /* pop architecture from name stack */
};

/* Initialization function for architecture :work:reg(reg_beh) */
int L4work_E3reg_init ();
int L3std_Q8standard_init ();
bool L4work_E3reg_A7reg_beh_init_done = false;
int L4work_E3reg_A7reg_beh_init(){
if (L4work_E3reg_A7reg_beh_init_done) return 1;
L4work_E3reg_A7reg_beh_init_done=true;
L4work_E3reg_init ();
L3std_Q8standard_init ();
register_source_file("/home/simo/My_Projects/Levitatore/MouseWheel/reg.vhdl","reg.vhdl");
name_stack iname;
iname.push("");
iname.pop();
return 1;
}

/* end of :work:reg(reg_beh) Architecture */
