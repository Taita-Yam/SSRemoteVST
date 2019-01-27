/**
 * GNU GENERAL PUBLIC LICENSE
 * Version 3, 29 June 2007
 *
 * Copyright © 2015
 * Quality and Usability Lab
 * Berlin Institute of Technology
 * http://www.qu.tu-berlin.de
 *
 * For more detailed information, please read the license.txt in the root directory.
 */

#include <src/controller.h>
 
//C++ Libs
#include <iostream>
#include <string>
#include <cmath>
#include <functional>
#include <memory>
#include <stdexcept>
#include <map>
#include <algorithm>


//JUCE Lib
#include <JuceLibraryCode/JuceHeader.h>

//Own Libs
//#include <src/utils/jack_client.h>
#include <src/utils/tcp_connection.h>
#include <src/utils/ssr_requester.h>
#include <src/utils/helper.h>
#include <src/utils/update_specificator.h>
#include <src/config/config.h>
#include <src/GUI/main_gui_component.h>
#include <src/scene/scene.h>
#include <src/scene/source.h>
#include <src/controller.h>
#include <src/utils/random_machine.h>
//#include <src/utils/logger.h>


//Boost Libs
#include <boost/filesystem.hpp>

#define LOG_TO_FILE 1

#define ERROR_MESSAGE 1

#if ERROR_MESSAGE

#define DEBUG_TCP_OUT 0
#define DEBUG_TCP_IN 0

#endif

/**
 * This creates new instances of the plugin..
 *
 * Called by that host!!
 */
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new Controller();
}

Controller::Controller()
	: ui_update_flag(true)
	, tcp_connection(nullptr)
	, message_to_ssr(new std::string(""))
	, message_from_ssr(new std::string(""))
	//, jack_client(new SSR::Jack_client())
	, requester(new SSR::SSR_requester())
	, config(nullptr)
	, scene(new SSR::Scene(30.0f))
	, treeState(*this, nullptr, "PARAMETERS", { std::make_unique<AudioParameterFloat>(X_ID, X_NAME,-30.0f, 30.0f, 0.0f)
											,	std::make_unique<AudioParameterFloat>(Y_ID, Y_NAME,-30.0f, 30.0f, 0.0f)
											,	std::make_unique<AudioParameterInt>(SOURCE_ID, SOURCE_NAME, 0, 14, 0)
												})


{
 
  //SSR::Logger::get_instance()->log(SSR::Logger::Level::ERRORz, "Constructor of Controller was called!", true);
  //SSR::Logger::get_instance()->log(SSR::Logger::Level::INFO, "Build: 6", false);

  boost::filesystem::path config_file = get_config_file_path();
  config = std::unique_ptr<SSR::Config>(new SSR::Config(config_file));
  config->load_config_xml_file();

  connect();

  //const char* jack_client_name = JucePlugin_Name;

  //try {
  //    jack_client->register_client(jack_client_name);
  //} catch (SSR::jack_server_not_running_exception& jsnre) {
  //    SSR::Logger::get_instance()->log(SSR::Logger::Level::ERRORz, jsnre.what(), LOG_TO_FILE);
  //} catch (std::exception& e) {
  //    SSR::Logger::get_instance()->log(SSR::Logger::Level::ERRORz, e.what(), LOG_TO_FILE);
  //}

  //	read_ssr_incoming_message();
}

Controller::~Controller()
{

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// The following methods are derived from the AudioProcessor base class
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const juce::String Controller::getName() const
{
  return JucePlugin_Name;
}

void Controller::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{

}

void Controller::releaseResources()
{

}

void Controller::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{

}

/*

const juce::String Controller::getInputChannelName(int channelIndex) const
{
  return juce::String(channelIndex + 1);
}

const juce::String Controller::getOutputChannelName(int channelIndex) const
{
  return juce::String(channelIndex + 1);
}

bool Controller::isInputChannelStereoPair(int index) const
{
  return true;
}

bool Controller::isOutputChannelStereoPair(int index) const
{
  return true;
}

bool Controller::silenceInProducesSilenceOut() const
{
  return false;
}

*/

double Controller::getTailLengthSeconds() const
{
  return 0.0;
}



bool Controller::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool Controller::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

AudioProcessorEditor* Controller::createEditor()
{
  return new Main_GUI_component(this);
}

bool Controller::hasEditor() const
{
  return true;
}




int Controller::getNumPrograms()
{
  return 1;
}

int Controller::getCurrentProgram()
{
  return 0;
}

void Controller::setCurrentProgram(int index)
{
  //There is no program to set.
}

const juce::String Controller::getProgramName (int index)
{
  return juce::String("Default SSRemote VST Program");
}

void Controller::changeProgramName(int index, const juce::String& newName)
{
  //There is no program which name can be changed.
}

void Controller::getStateInformation(MemoryBlock& destData)
{

	//std::unique_ptr <XmlElement> xml (treeState.state.createXml());
	//copyXmlToBinary(*xml, destData);

}

void Controller::setStateInformation(const void* data, int sizeInBytes)
{
	
	//std::unique_ptr <XmlElement> params_to_load(getXmlFromBinary(data, sizeInBytes));

	//if (params_to_load != nullptr)
	//{
		//if (params_to_load->hasTagName(treeState.state.getType()))
		//{
			//treeState.state = ValueTree::fromXml(*params_to_load);
		//}
	//}

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// The following methods are own implementations
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SSR::Source Controller::get_source() const
{
  return scene->get_source();
}

void Controller::set_x_position_discrete_of_selected_source(const float position)
{
  scene->set_x_position_discrete_of_selected_source(position);
  update_ssr(SSR::Update_specificator(SSR::Update_specificator::Specificators::position));
  ui_update_flag = true;
}

void Controller::set_y_position_discrete_of_selected_source(const float position)
{
  scene->set_y_position_discrete_of_selected_source(position);
  update_ssr(SSR::Update_specificator(SSR::Update_specificator::Specificators::position));
  ui_update_flag = true;
}

void Controller::set_gain_discrete_of_selected_source(const float gain, const bool linear)
{
  scene->set_gain_discrete_of_selected_source(gain, linear);
  update_ssr(SSR::Update_specificator(SSR::Update_specificator::Specificators::gain));
  ui_update_flag = true;
}

void Controller::set_mute_discrete_of_selected_source(const bool mute)
{
  scene->set_mute_discrete_of_selected_source(mute);
  update_ssr(SSR::Update_specificator(SSR::Update_specificator::mute));
  ui_update_flag = true;
}

void Controller::set_model_point_discrete_of_selected_source(const bool point)
{
  scene->set_model_point_discrete_of_selected_source(point);
  update_ssr(SSR::Update_specificator(SSR::Update_specificator::model));
  ui_update_flag = true;
}

void Controller::set_fixed_discrete_of_selected_source(const bool fixed)
{
  scene->set_fixed_discrete_of_selected_source(fixed);
  update_ssr(SSR::Update_specificator(SSR::Update_specificator::fixed));
  ui_update_flag = true;
}

void Controller::set_name_of_selected_source(const std::string name)
{
  scene->set_name_of_selected_source(name);
  update_ssr(SSR::Update_specificator(SSR::Update_specificator::name));
  ui_update_flag = true;
}

void Controller::set_properties_file_of_selected_source(const std::string value)
{
  scene->set_properties_file_of_selected_source(value);
  update_ssr(SSR::Update_specificator(SSR::Update_specificator::properties_file));
  ui_update_flag = true;
}

//void Controller::set_jackport_of_selected_source(const std::string jackport)
//{
//  scene->set_jackport_of_selected_source(jackport);
//  update_ssr(SSR::Update_specificator(SSR::Update_specificator::port));
//  ui_update_flag = true;
//}

bool Controller::select_source(const int id)
{
  return scene->select_source(id);
}

//void Controller::new_source()
//{
  //unsigned int id = scene->new_source("Source" + SSR::Random_machine::get_instance()->generate_string(5, 'A', 'Z'));
  //update_ssr(SSR::Update_specificator(SSR::Update_specificator::new_source));
  //ui_update_flag = true;
//}

bool Controller::read_ssr_incoming_message()
{
	int wait_in_msec = 1; // Try 10 or 100?

  bool message_incoming = false;

  while (tcp_connection->get_message(message_from_ssr, &wait_in_msec)) {
      scene->interpret_xml_message(*message_from_ssr);
      ui_update_flag = true;
      message_incoming = true;
  }

  return message_incoming;
}

void Controller::connect()
{
  config->load_config_xml_file();

  std::string hostname = config->get_network_config().get_hostname();
  unsigned int port = config->get_network_config().get_port();
  unsigned int timeout_in_ms = config->get_network_config().get_timeout();

  tcp_connection = std::unique_ptr<SSR::TCP_connection>(new SSR::TCP_connection(hostname, port, timeout_in_ms, '\0'));

  tcp_connection->connect();
}

void Controller::disconnect()
{
	tcp_connection->disconnect();
}

bool Controller::is_connected_to_ssr() const
{
  return tcp_connection->is_connected();
}

bool Controller::ui_needs_update() const
{
  return ui_update_flag;
}

void Controller::ui_request_update()
{
  ui_update_flag = true;
}

void Controller::ui_clear_update_flag()
{
  ui_update_flag = false;
}

/*
std::vector<std::string> Controller::get_all_jack_ports(const unsigned long flags)
{
  std::vector<std::string> available_jack_ports;
  available_jack_ports.push_back(std::string("No Jackport available."));

  try {
      available_jack_ports = jack_client->look_up_jack_ports(flags);
  } catch (SSR::jack_server_not_running_exception& jsnre) {
      SSR::Logger::get_instance()->log(SSR::Logger::Level::ERRORz, jsnre.what(), LOG_TO_FILE);
  } catch (std::exception& e) {
      SSR::Logger::get_instance()->log(SSR::Logger::Level::ERRORz, e.what(), LOG_TO_FILE);
  }

  return available_jack_ports;
}
*/

void Controller::update_ssr(SSR::Update_specificator specificator)
{

  switch (specificator.get_spec()) {

    case SSR::Update_specificator::Specificators::gain:

      //Importtant: Source Gain param is stored as linear value, so it has to be
      //converted to dB befor sending a request to change it
      *message_to_ssr = requester->build(std::bind(
          SSR::request::source_gain,
          std::placeholders::_1,
          scene->get_id_of_selected_source(),
          SSR::helper::linear_to_dB(scene->get_gain_of_selected_source().get_discrete_value())
      ));
      break;

    case SSR::Update_specificator::Specificators::position:
      *message_to_ssr = requester->build(std::bind(
          SSR::request::source_position,
          std::placeholders::_1,
          scene->get_id_of_selected_source(),
          scene->get_x_position_of_selected_source().get_discrete_value(),
          scene->get_y_position_of_selected_source().get_discrete_value()
      ));
      break;

    case SSR::Update_specificator::Specificators::mute:
      *message_to_ssr = requester->build(std::bind(
          SSR::request::source_mute,
          std::placeholders::_1,
          scene->get_id_of_selected_source(),
          scene->get_mute_of_selected_source().get_discrete_value()
      ));
      break;

    case SSR::Update_specificator::Specificators::fixed:
      *message_to_ssr = requester->build(std::bind(
          SSR::request::source_fixed,
          std::placeholders::_1,
          scene->get_id_of_selected_source(),
          scene->get_fixed_of_selected_source().get_discrete_value()
      ));
      break;

    case SSR::Update_specificator::Specificators::model:

      *message_to_ssr = requester->build(std::bind(
          SSR::request::source_model,
          std::placeholders::_1,
          scene->get_id_of_selected_source(),
          scene->get_model_point_of_selected_source().get_discrete_value() ? "point" : "plane"
      ));
      break;

    case SSR::Update_specificator::Specificators::name:

      *message_to_ssr = requester->build(std::bind(
          SSR::request::source_name,
          std::placeholders::_1,
          scene->get_id_of_selected_source(),
          scene->get_name_of_selected_source()
      ));
      break;

    case SSR::Update_specificator::Specificators::properties_file:
      *message_to_ssr = requester->build(std::bind(
          SSR::request::source_properties_file,
          std::placeholders::_1,
          scene->get_id_of_selected_source(),
          scene->get_properties_file_of_selected_source()
      ));
      break;

    //case SSR::Update_specificator::Specificators::port:
      //*message_to_ssr = requester->build(std::bind(
          //SSR::request::source_port,
          //std::placeholders::_1,
          //scene->get_id_of_selected_source(),
          //scene->get_jackport_of_selected_source()
      //));
      //break;

    //case SSR::Update_specificator::Specificators::new_source:
      //*message_to_ssr = requester->build(std::bind(
        //  SSR::request::new_source,
          //std::placeholders::_1,
          //scene->get_name_of_selected_source(),
          //scene->get_jackport_of_selected_source(),
          //scene->get_x_position_of_selected_source().get_discrete_value(),
          //scene->get_y_position_of_selected_source().get_discrete_value()
      //));
      //break;

    default:
      break;

  }

  send_message_to_ssr();
}

std::shared_ptr< std::vector< std::pair<unsigned int, std::string> > > Controller::get_source_ids_and_names()
{
  return scene->get_source_ids_and_names();
}

//==============================================================================
// PRIVATE METHODS
//==============================================================================

void Controller::send_message_to_ssr()
{

	tcp_connection->send_message(message_to_ssr, 0x01);

  //Reset message to empty string after sending the message
  *message_to_ssr = std::string("");
}

boost::filesystem::path Controller::get_config_file_path()
{
  namespace bfs = boost::filesystem;
  bfs::path config_file_location;

  try {
      config_file_location = SSR::helper::get_environment_variable("SSREMOTE_VST");
  } catch (std::invalid_argument& iae) {
      //SSR::Logger::get_instance()->log(SSR::Logger::Level::ERRORz, iae.what(), LOG_TO_FILE);
  }

  if (!bfs::is_directory(config_file_location)) {
      //SSR::Logger::get_instance()->log(SSR::Logger::Level::ERRORz, "Configfile location is not a directory!", LOG_TO_FILE);
  }

  std::string filename = "ssremote_config.xml";

  boost::filesystem::path config_file_path = config_file_location / bfs::path(filename);

  if (!bfs::is_regular_file(config_file_path)) {
      //SSR::Logger::get_instance()->log(SSR::Logger::Level::ERRORz, "Configfile is not a regular file!", LOG_TO_FILE);
  }

  return config_file_path;
}
























