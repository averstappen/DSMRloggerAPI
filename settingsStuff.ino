/*
***************************************************************************  
**  Program  : settingsStuff, part of DSMRfirmwareAPI
**  Version  : v0.0.1
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
* 1.0.11 added Mindergas Authtoken setting
*/

//=======================================================================
void writeSettings() 
{
  yield();
  DebugT(F("Writing to [")); Debug(SETTINGS_FILE); Debugln(F("] ..."));
  File file = SPIFFS.open(SETTINGS_FILE, "w"); // open for reading and writing
  if (!file) 
  {
    DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", SETTINGS_FILE);
    return;
  }
  yield();
  DebugT(F("Start writing setting data "));

  file.print("EnergyDeliveredT1 = "); file.println(String(settingEDT1, 5));     Debug(F("."));
  file.print("EnergyDeliveredT2 = "); file.println(String(settingEDT2, 5));     Debug(F("."));
  file.print("EnergyReturnedT1 = ");  file.println(String(settingERT1, 5));     Debug(F("."));
  file.print("EnergyReturnedT2 = ");  file.println(String(settingERT2, 5));     Debug(F("."));
  file.print("GASDeliveredT = ");     file.println(String(settingGDT,  5));     Debug(F("."));
  file.print("EnergyVasteKosten = "); file.println(String(settingENBK, 2));     Debug(F("."));
  file.print("GasVasteKosten = ");    file.println(String(settingGNBK, 2));     Debug(F("."));
  file.print("SleepTime = ");         file.println(settingSleepTime);           Debug(F("."));
  file.print("TelegramInterval = ");  file.println(settingInterval);            Debug(F("."));
  file.print("BackGroundColor = ");   file.println(settingBgColor);             Debug(F("."));
  file.print("FontColor = ");         file.println(settingFontColor);           Debug(F("."));

#ifdef USE_MQTT
  //sprintf(settingMQTTbroker, "%s:%d", MQTTbrokerURL, MQTTbrokerPort);
  file.print("MQTTbroker = ");        file.println(settingMQTTbroker);          Debug(F("."));
  file.print("MQTTUser = ");          file.println(settingMQTTuser);            Debug(F("."));
  file.print("MQTTpasswd = ");        file.println(settingMQTTpasswd);          Debug(F("."));
  file.print("MQTTinterval = ");      file.println(settingMQTTinterval);        Debug(F("."));
  file.print("MQTTtopTopic = ");      file.println(settingMQTTtopTopic);        Debug(F("."));
#endif
  
  file.print("MindergasAuthtoken = ");file.println(settingMindergasAuthtoken);  Debug(F("."));
  file.close();  
  
  Debugln(F(" done"));
  if (Verbose1) 
  {
    DebugTln(F("Wrote this:"));
    DebugT(F("EnergyDeliveredT1 = ")); Debugln(String(settingEDT1, 5));     
    DebugT(F("EnergyDeliveredT2 = ")); Debugln(String(settingEDT2, 5));     
    DebugT(F("EnergyReturnedT1 = "));  Debugln(String(settingERT1, 5));     
    DebugT(F("EnergyReturnedT2 = "));  Debugln(String(settingERT2, 5));     
    DebugT(F("GASDeliveredT = "));     Debugln(String(settingGDT,  5));     
    DebugT(F("EnergyVasteKosten = ")); Debugln(String(settingENBK, 2));    
    DebugT(F("GasVasteKosten = "));    Debugln(String(settingGNBK, 2));    
    DebugT(F("SleepTime = "));         Debugln(settingSleepTime);           
    DebugT(F("TelegramInterval = "));  Debugln(settingInterval);            
    DebugT(F("BackGroundColor = "));   Debugln(settingBgColor);             
    DebugT(F("FontColor = "));         Debugln(settingFontColor);   

#ifdef USE_MQTT
    //sprintf(settingMQTTbroker, "%s:%d", MQTTbrokerURL, MQTTbrokerPort);
    DebugT(F("MQTTbroker = "));        Debugln(settingMQTTbroker);          
    DebugT(F("MQTTUser = "));          Debugln(settingMQTTuser);     
  #ifdef SHOW_PASSWRDS       
      DebugT(F("MQTTpasswd = "));        Debugln(settingMQTTpasswd);  
  #else 
      DebugTln(F("MQTTpasswd = ********"));  
  #endif       
    DebugT(F("MQTTinterval = "));      Debugln(settingMQTTinterval);        
    DebugT(F("MQTTtopTopic = "));      Debugln(settingMQTTtopTopic);   
#endif
  
#ifdef USE_MINDERGAS
    DebugT(F("MindergasAuthtoken = "));Debugln(settingMindergasAuthtoken);  
#endif
  } // Verbose1
  
} // writeSettings()


//=======================================================================
void readSettings(bool show) 
{
  String sTmp, nColor;
  String words[10];
  File file;
  
  DebugTf(" %s ..\r\n", SETTINGS_FILE);

  settingEDT1       = 0.1;
  settingEDT2       = 0.2;
  settingERT1       = 0.3;
  settingERT2       = 0.4;
  settingGDT        = 0.5;
  settingENBK       = 15.15;
  settingGNBK       = 11.11;
  settingInterval   = 10; // seconds
  settingSleepTime  =  0; // infinite
  strcpy(settingBgColor, "deepskyblue");
  strcpy(settingFontColor, "white");
  settingMQTTbroker[0]     = '\0';
  settingMQTTuser[0]       = '\0';
  settingMQTTpasswd[0]     = '\0';
  settingMQTTinterval      = 60;
  sprintf(settingMQTTtopTopic, "%s", _HOSTNAME);
  settingMindergasAuthtoken[0] = '\0';

  if (!SPIFFS.exists(SETTINGS_FILE)) 
  {
    DebugTln(F(" .. file not found! --> created file!"));
    writeSettings();
  }

  for (int T = 0; T < 2; T++) 
  {
    file = SPIFFS.open(SETTINGS_FILE, "r");
    if (!file) 
    {
      if (T == 0) DebugTf(" .. something went wrong opening [%s]\r\n", SETTINGS_FILE);
      else        DebugT(T);
      delay(100);
    }
  } // try T times ..

  DebugTln(F("Reading settings:\r"));
  while(file.available()) 
  {
    sTmp                = file.readStringUntil('\n');
    sTmp.replace("\r", "");
    //DebugTf("[%s] (%d)\r\n", sTmp.c_str(), sTmp.length());
    int8_t wc = splitString(sTmp.c_str(), '=', words, 10);
    words[0].toLowerCase();
    nColor = words[1].substring(0,15);

    if (words[0].equalsIgnoreCase("EnergyDeliveredT1")) settingEDT1         = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("EnergyDeliveredT2")) settingEDT2         = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("EnergyReturnedT1"))  settingERT1         = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("EnergyReturnedT2"))  settingERT2         = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("GasDeliveredT"))     settingGDT          = words[1].toFloat();  
    if (words[0].equalsIgnoreCase("EnergyVasteKosten")) settingENBK         = words[1].toFloat();
    if (words[0].equalsIgnoreCase("GasVasteKosten"))    settingGNBK         = words[1].toFloat();

    if (words[0].equalsIgnoreCase("SleepTime"))         settingSleepTime    = words[1].toInt();  
    if (words[0].equalsIgnoreCase("TelegramInterval"))  settingInterval     = words[1].toInt();  

    //if (words[0].equalsIgnoreCase("BackgroundColor")) strcpy(settingBgColor,   String(nColor).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("BackgroundColor"))   strncpy(settingBgColor,   words[1].c_str(), (MAXCOLORNAME - 1));  
    //if (words[0].equalsIgnoreCase("FontColor"))       strcpy(settingFontColor, String(nColor).substring(0,(MAXCOLORNAME - 1)).c_str());  
    if (words[0].equalsIgnoreCase("FontColor"))         strncpy(settingFontColor,  words[1].c_str(), (MAXCOLORNAME - 1)); 

    if (words[0].equalsIgnoreCase("MindergasAuthtoken"))  strncpy(settingMindergasAuthtoken, words[1].c_str(), 20);  
   
#ifdef USE_MQTT
    if (words[0].equalsIgnoreCase("MQTTbroker"))  {
      memset(settingMQTTbroker, '\0', sizeof(settingMQTTbroker));
      memset(MQTTbrokerURL, '\0', sizeof(MQTTbrokerURL));
      //strcpy(settingMQTTbroker, String(words[1]).substring(0, 100).c_str());
      strncpy(settingMQTTbroker, words[1].c_str(), 100);
      int cln = String(settingMQTTbroker).indexOf(":");
      if (Verbose1) DebugTf("settingMQTTbroker[%s] => found[:] @[%d]\r\n", settingMQTTbroker, cln);
      if (cln > -1) 
      {
        //strcpy(MQTTbrokerURL, String(settingMQTTbroker).substring(0,cln).c_str());
        strncpy(MQTTbrokerURL, settingMQTTbroker, cln);
        DebugTf("->Port[%s]\r\n", String(settingMQTTbroker).substring((cln+1)).c_str());
        MQTTbrokerPort = String(settingMQTTbroker).substring((cln+1)).toInt();
      } 
      else 
      {
        strncpy(MQTTbrokerURL, settingMQTTbroker, 100);
        Debugln();
        MQTTbrokerPort = 1883;
      }
      DebugTf("=> MQTTbrokerURL[%s], port[%d]\r\n", MQTTbrokerURL, MQTTbrokerPort);
    }
    if (words[0].equalsIgnoreCase("MQTTuser"))          strncpy(settingMQTTuser    , words[1].c_str(), 35);  
    if (words[0].equalsIgnoreCase("MQTTpasswd"))        strncpy(settingMQTTpasswd  , words[1].c_str(), 25);  
    if (words[0].equalsIgnoreCase("MQTTinterval"))      settingMQTTinterval        = words[1].toInt();  
    if (words[0].equalsIgnoreCase("MQTTtopTopic"))      strncpy(settingMQTTtopTopic, words[1].c_str(), 20);  
#endif
    
  } // while available()

  file.close();  
  DebugTln(F(" .. done\r"));

  if (!show) return;
  
  Debugln(F("\r\n==== Settings ===================================================\r"));
  Debugf("   Energy Delivered Tarief 1 : %9.7f\r\n",  settingEDT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\r\n",  settingEDT2);
  Debugf("   Energy Delivered Tarief 1 : %9.7f\r\n",  settingERT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\r\n",  settingERT2);
  Debugf("        Gas Delivered Tarief : %9.7f\r\n",  settingGDT);
  Debugf("     Energy Netbeheer Kosten : %9.2f\r\n",  settingENBK);
  Debugf("        Gas Netbeheer Kosten : %9.2f\r\n",  settingGNBK);
  Debugf("   Telegram Process Interval : %d\r\n", settingInterval);
  Debugf("OLED Sleep Min. (0=oneindig) : %d\r\n", settingSleepTime);
  Debugf("            BackGround Color : %s\r\n", settingBgColor);
  Debugf("                  Font Color : %s\r\n", settingFontColor);
#ifdef USE_MQTT
  Debugln(F("\r\n==== MQTT settings ==============================================\r"));
  Debugf("          MQTT broker URL/IP : %s:%d", MQTTbrokerURL, MQTTbrokerPort);
  if (MQTTisConnected) Debugln(F(" (is Connected!)\r"));
  else                 Debugln(F(" (NOT Connected!)\r"));
  Debugf("                   MQTT user : %s\r\n", settingMQTTuser);
#ifdef SHOW_PASSWRDS
  Debugf("               MQTT password : %s\r\n", settingMQTTpasswd);
#else
  Debug( "               MQTT password : *************\r\n");
#endif
  Debugf("          MQTT send Interval : %d\r\n", settingMQTTinterval);
  Debugf("              MQTT top Topic : %s\r\n", settingMQTTtopTopic);
#endif  // USE_MQTT
#ifdef USE_MINDERGAS
  Debugln(F("\r\n==== Mindergas settings ==============================================\r"));
  Debugf("         Mindergas Authtoken : %s\r\n", settingMindergasAuthtoken);
#endif  
  
  Debugln(F("-\r"));

} // readSettings()


//=======================================================================
void writeColors() 
{
  yield();
  DebugTf(" %s .. ", GUI_COLORS_FILE);
  File file = SPIFFS.open(GUI_COLORS_FILE, "w"); // open for reading and writing
  if (!file) {
    Debugf("open(%s, 'w') FAILED!!! --> Bailout\r\n", GUI_COLORS_FILE);
    return;
  }
  yield();
  Debug(F("Start writing data .."));

  file.print("iniBordEDC = ");        file.println(iniBordEDC);
  file.print("iniFillEDC = ");        file.println(iniFillEDC);
  file.print("iniBordERC = ");        file.println(iniBordERC);
  file.print("iniFillERC = ");        file.println(iniFillERC);
  file.print("iniBordGDC = ");        file.println(iniBordGDC);
  file.print("iniFillGDC = ");        file.println(iniFillGDC);
  file.print("iniBordED2C = ");       file.println(iniBordED2C);
  file.print("iniFillED2C = ");       file.println(iniFillED2C);
  file.print("iniBordER2C = ");       file.println(iniBordER2C);
  file.print("iniFillER2C = ");       file.println(iniFillER2C);
  file.print("iniBordGD2C = ");       file.println(iniBordGD2C);
  file.print("iniFillGD2C = ");       file.println(iniFillGD2C);
  file.print("iniBordPR123C = ");     file.println(iniBordPR123C);
  file.print("iniFillPR123C = ");     file.println(iniFillPR123C);
  file.print("iniBordPD1C = ");       file.println(iniBordPD1C);
  file.print("iniFillPD1C = ");       file.println(iniFillPD1C);
  file.print("iniBordPD2C = ");       file.println(iniBordPD2C);
  file.print("iniFillPD2C = ");       file.println(iniFillPD2C);
  file.print("iniBordPD3C = ");       file.println(iniBordPD3C);
  file.print("iniFillPD3C = ");       file.println(iniFillPD3C);

  file.close();  
  
  Debugln(F(" .. done\r"));

} // writeColors()


//=======================================================================
void readColors(bool show) 
{
  String sTmp, nColor;
  String words[10];

  DebugTf(" %s ..", GUI_COLORS_FILE);

  strcpy(iniFillEDC   , "red");
  strcpy(iniBordEDC   , "red");
  strcpy(iniFillERC   , "green");
  strcpy(iniBordERC   , "green");
  strcpy(iniFillGDC   , "blue");
  strcpy(iniBordGDC   , "blue");
  strcpy(iniFillED2C  , "tomato");
  strcpy(iniBordED2C  , "tomato");
  strcpy(iniFillER2C  , "lightgreen");
  strcpy(iniBordER2C  , "lightgreen");
  strcpy(iniFillGD2C  , "lightblue");
  strcpy(iniBordGD2C  , "lightblue");
  strcpy(iniFillPR123C, "green");
  strcpy(iniBordPR123C, "green");
  strcpy(iniFillPD1C  , "yellow");
  strcpy(iniBordPD1C  , "yellow");
  strcpy(iniFillPD2C  , "lightgreen");
  strcpy(iniBordPD2C  , "lightgreen");
  strcpy(iniFillPD3C  , "lime");
  strcpy(iniBordPD3C  , "lime");

  if (!SPIFFS.exists(GUI_COLORS_FILE)) 
  {
    Debugln(F(" .. file not found! --> created file!\r"));
    writeColors();
  }

  File file = SPIFFS.open(GUI_COLORS_FILE, "r");

  //Debugln(F("\r"));
  while(file.available()) 
  {
    sTmp                = file.readStringUntil('\n');
    sTmp.replace("\r", "");
    //DebugTf("[%s] (%d)\r\n", sTmp.c_str(), sTmp.length());
    int8_t wc = splitString(sTmp.c_str(), '=', words, 10);
    words[0].toLowerCase();
    nColor = words[1].substring(0,15);

    if (words[0].equalsIgnoreCase("iniBordEDC"))      strncpy(iniBordEDC   , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillEDC"))      strncpy(iniFillEDC   , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordERC"))      strncpy(iniBordERC   , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillERC"))      strncpy(iniFillERC   , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordGDC"))      strncpy(iniBordGDC   , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillGDC"))      strncpy(iniFillGDC   , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordED2C"))     strncpy(iniBordED2C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillED2C"))     strncpy(iniFillED2C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordER2C"))     strncpy(iniBordER2C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillER2C"))     strncpy(iniFillER2C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordGD2C"))     strncpy(iniBordGD2C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillGD2C"))     strncpy(iniFillGD2C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordPR123C"))   strncpy(iniBordPR123C, words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillPR123C"))   strncpy(iniFillPR123C, words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordPD1C"))     strncpy(iniBordPD1C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillPD1C"))     strncpy(iniFillPD1C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordPD2C"))     strncpy(iniBordPD2C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillPD2C"))     strncpy(iniFillPD2C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniBordPD3C"))     strncpy(iniBordPD3C  , words[1].c_str(), (MAXCOLORNAME -1));  
    if (words[0].equalsIgnoreCase("iniFillPD3C"))     strncpy(iniFillPD3C  , words[1].c_str(), (MAXCOLORNAME -1));  

    yield();
    
  } // while available()
  
  file.close();  
  Debugln(F(" .. done\r"));

  if (!show) return;

  Debugln(F("\r\n==== Chart colors ===============================================\r"));
  Debugf("  Energie Verbruik LineColor : %s\r\n", iniBordEDC);  
  Debugf("  Energie Verbruik BackColor : %s\r\n", iniFillEDC);  
  Debugf("  Energie Returned LineColor : %s\r\n", iniBordERC);  
  Debugf("  Energie Returned BackColor : %s\r\n", iniFillERC);  
  Debugf("      Gas Verbruik LineColor : %s\r\n", iniBordGDC);  
  Debugf("      Gas Verbruik BackColor : %s\r\n", iniFillGDC);  
  Debugf("Energie Verbruik H LineColor : %s\r\n", iniBordED2C);  
  Debugf("Energie Verbruik H BackColor : %s\r\n", iniFillED2C);  
  Debugf("Energie Returned H LineColor : %s\r\n", iniBordER2C);  
  Debugf("Energie Returned H BackColor : %s\r\n", iniFillER2C);  
  Debugf("    Gas Verbruik H LineColor : %s\r\n", iniBordGD2C);  
  Debugf("    Gas Verbruik H BackColor : %s\r\n", iniFillGD2C);  
  Debugf(" Power Verbruik L1 LineColor : %s\r\n", iniBordPD1C);  
  Debugf(" Power Verbruik L1 BackColor : %s\r\n", iniFillPD1C);  
  Debugf(" Power Verbruik L2 LineColor : %s\r\n", iniBordPD2C);  
  Debugf(" Power Verbruik L2 BackColor : %s\r\n", iniFillPD2C);  
  Debugf(" Power Verbruik L3 LineColor : %s\r\n", iniBordPD3C);
  Debugf(" Power Verbruik L3 BackColor : %s\r\n", iniFillPD3C);  
  Debugf("   Power Ret. L123 LineColor : %s\r\n", iniBordPR123C);  
  Debugf("   Power Ret. L123 BackColor : %s\r\n", iniFillPR123C);  
  
  Debugln(F("-\r"));

} // readColors()

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************/
