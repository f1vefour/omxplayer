#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <libcec/cec.h>

using namespace std;
using namespace CEC;

#define CEC_CONFIG_VERSION CEC_CLIENT_VERSION_2_0_2;

#include <libcec/cecloader.h>

// bool                 g_bExit(false);

// //get the first word (separated by whitespace) from string data and place that in word
// //then remove that word from string data
// bool GetWord(string& data, string& word)
// {
//   stringstream datastream(data);
//   string end;

//   datastream >> word;
//   if (datastream.fail())
//   {
//     data.clear();
//     return false;
//   }

//   size_t pos = data.find(word) + word.length();

//   if (pos >= data.length())
//   {
//     data.clear();
//     return true;
//   }

//   data = data.substr(pos);

//   datastream.clear();
//   datastream.str(data);

//   datastream >> end;
//   if (datastream.fail())
//     data.clear();

//   return true;
// }

// bool ProcessConsoleCommand(ICECAdapter *parser, string &input)
// {
//   if (!input.empty())
//   {
//     string command;
//     if (GetWord(input, command))
//     {
//       if (command == "q" || command == "quit")
//         return false;

//       // ProcessCommandTX(parser, command, input) ||
//       // ProcessCommandON(parser, command, input) ||
//       // ProcessCommandSTANDBY(parser, command, input) ||
//       // ProcessCommandPOLL(parser, command, input) ||
//       // ProcessCommandLA(parser, command, input) ||
//       // ProcessCommandP(parser, command, input) ||
//       // ProcessCommandPA(parser, command, input) ||
//       // ProcessCommandAS(parser, command, input) ||
//       // ProcessCommandOSD(parser, command, input) ||
//       // ProcessCommandPING(parser, command, input) ||
//       // ProcessCommandVOLUP(parser, command, input) ||
//       // ProcessCommandVOLDOWN(parser, command, input) ||
//       // ProcessCommandMUTE(parser, command, input) ||
//       // ProcessCommandMON(parser, command, input) ||
//       // ProcessCommandBL(parser, command, input) ||
//       // ProcessCommandLANG(parser, command, input) ||
//       // ProcessCommandVEN(parser, command, input) ||
//       // ProcessCommandVER(parser, command, input) ||
//       // ProcessCommandPOW(parser, command, input) ||
//       // ProcessCommandNAME(parser, command, input) ||
//       // ProcessCommandLAD(parser, command, input) ||
//       // ProcessCommandAD(parser, command, input) ||
//       // ProcessCommandAT(parser, command, input) ||
//       // ProcessCommandR(parser, command, input) ||
//       // ProcessCommandH(parser, command, input) ||
//       // ProcessCommandLOG(parser, command, input) ||
//       // ProcessCommandSCAN(parser, command, input) ||
//       // ProcessCommandSP(parser, command, input) ||
//       // ProcessCommandSPL(parser, command, input) ||
//       // ProcessCommandSELF(parser, command, input);
//     }
//   }
//   return true;
// }

// int CecLogMessage(void *cbParam, const cec_log_message message)
// {
//   if ((message.level & g_cecLogLevel) == message.level)
//   {
//     string strLevel;
//     switch (message.level)
//     {
//     case CEC_LOG_ERROR:
//       strLevel = "ERROR:   ";
//       break;
//     case CEC_LOG_WARNING:
//       strLevel = "WARNING: ";
//       break;
//     case CEC_LOG_NOTICE:
//       strLevel = "NOTICE:  ";
//       break;
//     case CEC_LOG_TRAFFIC:
//       strLevel = "TRAFFIC: ";
//       break;
//     case CEC_LOG_DEBUG:
//       strLevel = "DEBUG:   ";
//       break;
//     default:
//       break;
//     }

//     //printf("%s[%16lld]\t%s\n", strLevel.c_str(), message.time, message.message);
//   }

//   return 0;
// }

// int CecKeyPress(void *cbParam, const cec_keypress key)
// {
//   switch(key.keycode)
//   {
//     case CEC_USER_CONTROL_CODE_PLAY:
//       puts("play");
//       break;
//     case CEC_USER_CONTROL_CODE_PAUSE:
//       puts("pause");
//       break;
//     // case CEC_USER_CONTROL_CODE_STOP:
//     //   puts("stop");
//     //   break;
//     case CEC_USER_CONTROL_CODE_BACKWARD:
//       puts("backward");
//       break;
//     case CEC_USER_CONTROL_CODE_FORWARD:
//       puts("forward");
//       break;
//     default:
//       printf("keycode 0x%X\n", key.keycode);
//   }
//   printf("duration: %i\n", key.duration);
//   return 0;
// }

// int CecCommand(void *cbParam, const cec_command command)
// {
//   return 0;
// }

// int CecAlert(void *cbParam,
//              const libcec_alert type,
//              const libcec_parameter param)
// {
//   switch (type)
//   {
//   case CEC_ALERT_CONNECTION_LOST:
//     cout << "Connection lost - exiting\n" << endl;
//     g_bExit = true;
//     break;
//   default:
//     break;
//   }
//   return 0;
// }

class SimpleCEC
{
public:
  SimpleCEC(const SimpleCEC&) = delete;
  SimpleCEC& operator=(const SimpleCEC&) = delete;

  SimpleCEC()
  : m_open(),
    m_keyPress(-1),
    g_config(),
    g_callbacks()
  {
    g_config.Clear();
    g_callbacks.Clear();
    snprintf(g_config.strDeviceName, 13, "CECTester");
    g_config.clientVersion       = CEC_CONFIG_VERSION;
    g_config.bActivateSource     = 0;
    g_callbacks.CBCecLogMessage  = &CecLogMessage;
    g_callbacks.CBCecKeyPress    = &CecKeyPress;
    g_callbacks.CBCecCommand     = &CecCommand;
    g_callbacks.CBCecAlert       = &CecAlert;
    g_config.callbacks           = &g_callbacks;
    g_config.callbackParam       = this;

    if (g_config.deviceTypes.IsEmpty())
    {
      g_config.deviceTypes.Add(CEC_DEVICE_TYPE_RECORDING_DEVICE);
    }

    parser = LibCecInitialise(&g_config);
    if (!parser)
    {
      cout << "Cannot load libcec.so" << endl;

      if (parser)
        UnloadLibCec(parser);

      return;
    }

    parser->InitVideoStandalone();

    printf("CEC Parser created - libCEC version %s\n",
                  parser->ToString((cec_server_version)g_config.serverVersion));

    cec_adapter devices[10];
    uint8_t iDevicesFound = parser->FindAdapters(devices, 10, NULL);
    if (iDevicesFound <= 0)
    {
      cout << "autodetect FAILED" << endl;
      UnloadLibCec(parser);
      return;
    }
    else
    {
      cout << endl << " path:     " << devices[0].path << endl <<
            " com port: " << devices[0].comm << endl << endl;
      g_strPort = devices[0].comm;
    }

    cout << "opening a connection to the CEC adapter..." << endl;

    if (!parser->Open(g_strPort.c_str()))
    {
      cout << "unable to open the device on port " << g_strPort.c_str() << endl;
      UnloadLibCec(parser);
      return;
    }

    m_open = true;
  }

  ~SimpleCEC()
  {
    if(!m_open) return;

    parser->Close();
    UnloadLibCec(parser);
    parser = NULL;
    m_open = false;
  }

  int getKeyPress() {
    assert(open());
    int kp = m_keyPress;
    m_keyPress = -1;
    return kp;
  }

  bool open() {return m_open;}

private:

  static int CecLogMessage(void *cbParam, const cec_log_message message)
  {
    return 0;
  }

  static int CecKeyPress(void *cbParam, const cec_keypress key)
  {
    return 0;
  }

  static int CecCommand(void *cbParam, const cec_command command)
  {
    if(command.opcode == CEC_OPCODE_USER_CONTROL_PRESSED)
    {
      auto self = (SimpleCEC*) cbParam;
      self->m_keyPress = command.parameters[0];
    }
    return 0;
  }

  static int CecAlert(void *cbParam,
                      const libcec_alert type,
                      const libcec_parameter param)
  {
    return 0;
  }

  bool                 m_open;
  atomic<int>          m_keyPress;
  ICECCallbacks        g_callbacks;
  libcec_configuration g_config;
  string               g_strPort;
  ICECAdapter *parser;
};

// int main() {
//   SimpleCEC cec;

//   while (!g_bExit)
//   {
//     string input;
//     getline(cin, input);
//     cin.clear();

//     if (ProcessConsoleCommand(NULL, input) && !g_bExit)
//     {
//       if (!input.empty())
//         cout << "waiting for input" << endl;
//     }
//     else
//       g_bExit = true;

//     int kp = cec.getKeyPress();
//     if (kp != -1)
//       printf("keypress: %i\n", kp);

//     if (!g_bExit)
//       this_thread::sleep_for(chrono::milliseconds(100));
//   }

//   return 0;
// }