#include "Initiator.h"

Initiator::Initiator(sc_module_name n) : sc_module(n), i_skt("i_skt") {}

int Initiator::read_from_socket(unsigned long int addr, unsigned char mask[],
                                unsigned char rdata[], int dataLen) {
  delay = m_qk.get_local_time();
  
  // Set up the payload fields. Assume everything is 4 bytes.
  trans.set_read();
  trans.set_address((sc_dt::uint64)addr);

  trans.set_byte_enable_length((const unsigned int)dataLen);
  trans.set_byte_enable_ptr((unsigned char *)mask);

  trans.set_data_length((const unsigned int)dataLen);
  trans.set_data_ptr((unsigned char *)rdata);

  // Transport.
  do_trans(trans);

  // Increment local time with delay returned from b_transport()
  //m_qk.set( delay );
  //m_qk.inc( sc_time(10, SC_NS) );
  //  m_qk.inc( delay );
  // Check if synchronize is necessary
  //  if (m_qk.need_sync()) m_qk.sync();

  /* For now just simple non-zero return code on error */
  return trans.is_response_ok() ? 0 : -1;

} // read_from_socket()

int Initiator::write_to_socket(unsigned long int addr, unsigned char mask[],
                               unsigned char wdata[], int dataLen) {
  // Set up the payload fields. Assume everything is 4 bytes.
  delay = m_qk.get_local_time();
  //tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND; 
  trans.set_write();
  trans.set_address((sc_dt::uint64)addr);

  trans.set_byte_enable_length((const unsigned int)dataLen);
  trans.set_byte_enable_ptr((unsigned char *)mask);

  trans.set_data_length((const unsigned int)dataLen);
  trans.set_data_ptr((unsigned char *)wdata);

  // Transport.
  do_trans(trans);
  // Increment local time with delay returned from b_transport()
  //init_socket->b_transport( trans, delay );
  //m_qk.set( delay );
  //m_qk.inc( sc_time(10, SC_NS) );
  //m_qk.inc( delay );
  // Check if synchronize is necessary
  //if (m_qk.need_sync()) m_qk.sync();

  /* For now just simple non-zero return code on error */
  return trans.is_response_ok() ? 0 : -1;

} // writeUpcall()

void Initiator::do_trans(tlm::tlm_generic_payload &trans) {
  sc_core::sc_time dummyDelay = sc_core::SC_ZERO_TIME;
  delay = m_qk.get_local_time();
  //m_qk.set_global_quantum( sc_time(1, SC_NS) );
  //m_qk.reset();

  // Call the transport and wait for no time, which allows the thread to yield
  // and others to get a look in!
  m_qk.set( delay );
  m_qk.inc( sc_time(1, SC_NS) );
  //m_qk.inc( delay );
  // Check if synchronize is necessary
  if (m_qk.need_sync()) m_qk.sync();
  i_skt->b_transport(trans, dummyDelay);
  //wait(sc_core::SC_ZERO_TIME);
  wait(dummyDelay);

} // do_trans()


