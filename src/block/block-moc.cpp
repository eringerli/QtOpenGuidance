// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "moc_AckermannSteering.cpp"
#include "moc_BlockBase.cpp"
#include "moc_CameraController.cpp"
#include "moc_CommunicationJrk.cpp"
#include "moc_CommunicationPgn7FFE.cpp"
#include "moc_DebugSink.cpp"
#include "moc_FieldManager.cpp"
#include "moc_FileStream.cpp"
#include "moc_FpsMeasurement.cpp"
#include "moc_GlobalPlannerLines.cpp"
#include "moc_GridModel.cpp"
#include "moc_GuidanceGlobalPlannerModel.cpp"
#include "moc_GuidanceLocalPlanner.cpp"
#include "moc_GuidancePlannerGui.cpp"
#include "moc_GuidanceStanley.cpp"
#include "moc_GuidanceXte.cpp"
#include "moc_Implement.cpp"
#include "moc_ImplementSection.cpp"
#include "moc_NmeaParserGGA.cpp"
#include "moc_NmeaParserHDT.cpp"
#include "moc_NmeaParserRMC.cpp"
#include "moc_NumberObject.cpp"
#include "moc_OrientationDockBlock.cpp"
#include "moc_PoseSimulation.cpp"
#include "moc_PoseSynchroniser.cpp"
#include "moc_PositionDockBlock.cpp"
#include "moc_PrintLatency.cpp"
#include "moc_SprayerModel.cpp"
#include "moc_StringObject.cpp"
#include "moc_TractorModel.cpp"
#include "moc_TrailerModel.cpp"
#include "moc_TransverseMercatorConverter.cpp"
#include "moc_UdpSocket.cpp"
#include "moc_ValueDockBlockBase.cpp"
#include "moc_ValueDockBlock.cpp"
#include "moc_VectorObject.cpp"
#include "moc_XteDockBlock.cpp"

#ifdef SERIALPORT_ENABLED
#include "moc_SerialPort.cpp"
#endif
