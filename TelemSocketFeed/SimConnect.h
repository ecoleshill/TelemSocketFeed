#pragma once
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
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>


namespace _SimConnect
{
	const enum DataTypes { tBOOL, tINT, tFLOAT };
	struct DataElement
	{
		DataTypes Type;			//The type of data stored in the 4-bytes of data
		std::string Name;		//Name of the data
		std::string Units;		//The units of the data type
		char Data[4];			//4-bytes of RAW data
	};

	class CSimConnect
	{
	public:
		CSimConnect(std::string Filename);		//Constructor loads configuration file

		void Run();
	};
}