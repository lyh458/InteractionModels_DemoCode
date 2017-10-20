//  Robot Raconteur(R) - A communication library for robotics and automation systems
//  Copyright (C) 2014 John Wason <wason@wasontech.com>
//                     Wason Technology, LLC
//
//  This program is released under the terms of the Robot Raconteur(R)
//  license.  Full text can be found at  http://robotraconteur.com/license2 .
//  Attribute to this library as "Robot Raconteur(R)" in documentation or 
//  packaging.  This software is royalty free for commercial use under
//  the conditions of the license.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

#pragma once

#include <stdint.h>


namespace RobotRaconteur
{



	enum DataTypes
	{
		DataTypes_void_t = 0,
		DataTypes_double_t,
		DataTypes_single_t,
		DataTypes_int8_t,
		DataTypes_uint8_t,
		DataTypes_int16_t,
		DataTypes_uint16_t,
		DataTypes_int32_t,
		DataTypes_uint32_t,
		DataTypes_int64_t,
		DataTypes_uint64_t,
		DataTypes_string_t,
		DataTypes_structure_t = 101,
		DataTypes_vector_t,
		DataTypes_dictionary_t,
		DataTypes_object_t,
		DataTypes_varvalue_t,
		DataTypes_varobject_t,
		DataTypes_multidimarray_t,
		DataTypes_list_t
		
	};



	enum MessageEntryType
	{

		MessageEntryType_Null = 0,
		MessageEntryType_StreamOp = 1,
		MessageEntryType_StreamOpRet,
		MessageEntryType_StreamCheckCapability,
		MessageEntryType_StreamCheckCapabilityRet,
		MessageEntryType_GetServiceDesc = 101,
		MessageEntryType_GetServiceDescRet,
		MessageEntryType_ObjectTypeName,
		MessageEntryType_ObjectTypeNameRet,
		MessageEntryType_ServiceClosed,
		MessageEntryType_ServiceClosedRet,
		MessageEntryType_ConnectClient,
		MessageEntryType_ConnectClientRet,
		MessageEntryType_DisconnectClient,
		MessageEntryType_DisconnectClientRet,
		MessageEntryType_ConnectionTest,
		MessageEntryType_ConnectionTestRet,
		MessageEntryType_GetNodeInfo,
		MessageEntryType_GetNodeInfoRet,
		MessageEntryType_ReconnectClient,
		MessageEntryType_ReconnectClientRet,
		MessageEntryType_NodeCheckCapability,
		MessageEntryType_NodeCheckCapabilityRet,
		MessageEntryType_GetServiceAttributes,
		MessageEntryType_GetServiceAttributesRet,
		MessageEntryType_EndpointCheckCapability = 501,
		MessageEntryType_EndpointCheckCapabilityRet,
		MessageEntryType_ServiceCheckCapabilityReq = 1101,
		MessageEntryType_ServiceCheckCapabilityRet,
		MessageEntryType_ClientKeepAliveReq = 1105,
		MessageEntryType_ClientKeepAliveRet,
		MessageEntryType_ClientSessionOpReq = 1107,
		MessageEntryType_ClientSessionOpRet,
		MessageEntryType_ServicePathReleasedReq,
		MessageEntryType_ServicePathReleasedRet,
		MessageEntryType_PropertyGetReq = 1111,
		MessageEntryType_PropertyGetRes,
		MessageEntryType_PropertySetReq,
		MessageEntryType_PropertySetRes,
		MessageEntryType_FunctionCallReq = 1121,
		MessageEntryType_FunctionCallRes,
		MessageEntryType_EventReq = 1131,
		MessageEntryType_EventRes,
		MessageEntryType_PipePacket = 1141,
		MessageEntryType_PipePacketRet,
		MessageEntryType_PipeConnectReq,
		MessageEntryType_PipeConnectRet,
		MessageEntryType_PipeDisconnectReq,
		MessageEntryType_PipeDisconnectRet,
		MessageEntryType_PipeClosed,
		MessageEntryType_PipeClosedRet,
		MessageEntryType_CallbackCallReq = 1151,
		MessageEntryType_CallbackCallRet,
		MessageEntryType_WirePacket = 1161,
		MessageEntryType_WirePacketRet,
		MessageEntryType_WireConnectReq,
		MessageEntryType_WireConnectRet,
		MessageEntryType_WireDisconnectReq,
		MessageEntryType_WireDisconnectRet,
		MessageEntryType_WireClosed,
		MessageEntryType_WireClosedRet,
		MessageEntryType_MemoryRead = 1171,
		MessageEntryType_MemoryReadRet,
		MessageEntryType_MemoryWrite,
		MessageEntryType_MemoryWriteRet,
		MessageEntryType_MemoryGetParam,
		MessageEntryType_MemoryGetParamRet,

		//Dedicated transport message types.  These are an extension to the protocol
		//and not a base feature
		MessageEntryType_WireTransportOpReq = 11161,
		MessageEntryType_WireTransportOpRet,
		MessageEntryType_WireTransportEvent,
		MessageEntryType_WireTransportEventRet

	};

	enum MessageErrorType
	{

		MessageErrorType_None = 0,
		MessageErrorType_ConnectionError = 1,
		MessageErrorType_ProtocolError,
		MessageErrorType_ServiceNotFound,
		MessageErrorType_ObjectNotFound,
		MessageErrorType_InvalidEndpoint,
		MessageErrorType_EndpointCommunicationFatalError,
		MessageErrorType_NodeNotFound,
		MessageErrorType_ServiceError,
		MessageErrorType_MemberNotFound,
		MessageErrorType_MemberFormatMismatch,
		MessageErrorType_DataTypeMismatch,
		MessageErrorType_DataTypeError,
		MessageErrorType_DataSerializationError,
		MessageErrorType_MessageEntryNotFound,
		MessageErrorType_MessageElementNotFound,
		MessageErrorType_UnknownError,
		MessageErrorType_RemoteError = 100,
		MessageErrorType_TransactionTimeout,
		MessageErrorType_AuthenticationError = 150,
		MessageErrorType_ObjectLockedError

	};

	enum ClientServiceListenerEventType
	{
		ClientServiceListenerEventType_ClientClosed = 1,

		ClientServiceListenerEventType_ClientConnectionTimeout
	};

	enum ServerServiceListenerEventType
	{
		ServerServiceListenerEventType_ServiceClosed = 1,
		ServerServiceListenerEventType_ClientConnected,
		ServerServiceListenerEventType_ClientDisconnected

	};

#define RR_TIMEOUT_INFINITE -1
}
