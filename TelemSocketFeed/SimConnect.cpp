/****************************************************************************************
Module:		SimConnect
Author:		Dr. (James) Elliott Coleshill, Ph.D, P.Eng
Purpose:	This module provides the interface to the SimConnect libraries.  It contains
a configuration load to specify the parameters, links and loads the SimConnect
libaray files.  It also handles the control thread for collecting
and storing telemetry received by the SimConnect library.

History:
2018-10-09:	Initial Release
*****************************************************************************************/
#include "SimConnect.h"


namespace _SimConnect
{
	std::vector<DataElement> SimData;	//Vector space that stores the simulation data configured by the CFG input file

	CSimConnect::CSimConnect(std::string Filename)
	{
		std::string ReadLine;				//Line of text read from the configuration file
		std::string ParamType;				//Parameter type information parsed from the CFG file
		int CurrLoc, LastLoc;				//Location index values into the ReadLine string
		bool bGood;							//Flag to determine if data have a correct data type

		std::ifstream *ifs = new std::ifstream(Filename);
		if (ifs->is_open())
		{
			//Read all the data, create the vector space and configure the Telem GUI
			while (!ifs->eof())
			{
				bGood = true;							//Assume all is okay until data type is validated
				DataElement NewElement;					//The new data element to be added to the vector space

				memset(NewElement.Data, 0, sizeof(float));	//Init the data to zero

				std::getline(*ifs, ReadLine);

				//Parse the data using CSV format
				CurrLoc = ReadLine.find(",");
				NewElement.Name = ReadLine.substr(0, CurrLoc);
				LastLoc = CurrLoc;
				CurrLoc = ReadLine.find(",", LastLoc + 1);
				NewElement.Units = ReadLine.substr(LastLoc + 1, CurrLoc - (LastLoc + 1));
				ParamType = ReadLine.substr(CurrLoc + 1, ReadLine.length());

				if (ParamType == "BOOL")
					NewElement.Type = tBOOL;
				else if (ParamType == "INT")
					NewElement.Type = tINT;
				else if (ParamType == "FLOAT")
					NewElement.Type = tFLOAT;
				else
				{
					std::string MsgToWrite = NewElement.Name + "Have Invalid Type: " + ParamType;
					std::cout << MsgToWrite << std::endl;
					bGood = false;
				}

				if (bGood)
				{
					SimData.push_back(NewElement);
				}
			}

			ifs->close();
		}
		else
		{
			//Log the failure to open the configuration file to the system logger
			std::string MsgToSend = "Unable to open file: " + Filename;
			std::cerr << MsgToSend << std::endl;
		}
	}

	void CSimConnect::Run()
	{

	}
}