#pragma once

#include "eyeXHost.h"
#include <windows.h>
#include <vector>
#include <mutex>
#include <cassert>
#include <cstdint>
#include "stdafx.h"



#pragma comment (lib, "Tobii.EyeX.Client.lib")

// ID of the global interactor that provides our data stream; must be unique within the application.
static const TX_STRING InteractorId = "SomethingSomethingName";

// global variables
static TX_HANDLE g_hGlobalInteractorSnapshot = TX_EMPTY_HANDLE;

/*
 * Initializes g_hGlobalInteractorSnapshot with an interactor that has the Gaze Point behavior.
 */
bool eyeXHost::InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext) {
	TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
	TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED };
	bool success;

	success = txCreateGlobalInteractorSnapshot(
		hContext,
		InteractorId,
		&g_hGlobalInteractorSnapshot,
		&hInteractor) == TX_RESULT_OK;
	success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;

	txReleaseObject(&hInteractor);

	return success;
}

/*
 * Callback function invoked when the status of the connection to the EyeX Engine has changed.
 */
void TX_CALLCONVENTION eyeXHost::OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState) {
	string connectionStateMessage;
	auto snapshotCommittedTrampoline = [](TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam) {
		static_cast<eyeXHost*>(userParam)->OnSnapshotCommitted(hAsyncData);
	};
	
	switch (connectionState) {
	case TX_CONNECTIONSTATE_CONNECTED: {
			bool success;
			cout << "The connection state is now CONNECTED (We are connected to the EyeX Engine)" << "\n";
			success = txCommitSnapshotAsync(g_hGlobalInteractorSnapshot, snapshotCommittedTrampoline, NULL) == TX_RESULT_OK;
			if (!success) {
				cout << "Failed to initialize the data stream." << "\n";
			}
			else {
				cout << "Waiting for gaze data to start streaming..." << "\n";
			}
		}
		break;

	case TX_CONNECTIONSTATE_DISCONNECTED:
		cout << "TX_CONNECTIONSTATE_DISCONNECTED" << "\n";
		break;

	case TX_CONNECTIONSTATE_TRYINGTOCONNECT:
		cout << "TX_CONNECTIONSTATE_TRYINGTOCONNECT" << "\n";
		break;

	case TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW:
		cout << "TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW" << "\n";
		break;

	case TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH:
		cout << "TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH" << "\n";
		break;
	}
}

/*
 * Callback function invoked when a snapshot has been committed.
 */
void TX_CALLCONVENTION eyeXHost::OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData) {
	TX_RESULT result = TX_RESULT_UNKNOWN;
	txGetAsyncDataResultCode(hAsyncData, &result);
	assert(result == TX_RESULT_OK || result == TX_RESULT_CANCELLED);
}

bool eyeXHost::RegisterConnectionStateChangedHandler() {
		auto connectionStateChangedTrampoline = [](TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam) {
			static_cast<eyeXHost*>(userParam)->OnEngineConnectionStateChanged(connectionState);
		};
		bool success = txRegisterConnectionStateChangedHandler(_context, &_connectionStateChangedTicket, connectionStateChangedTrampoline, this) == TX_RESULT_OK;
		return success;
}

/* 
 * Handles an event from the Gaze Point data stream.
 */
void eyeXHost::OnGazeDataEvent(TX_HANDLE hGazeDataBehavior) {
	
	TX_GAZEPOINTDATAEVENTPARAMS eventParams;
	if (txGetGazePointDataEventParams(hGazeDataBehavior, &eventParams) == TX_RESULT_OK) {
		gazePoint.set(eventParams.X, eventParams.Y);
	} else {
		cout << "Failed to interpret gaze data event packet" << "\n";
	}
}

void TX_CALLCONVENTION eyeXHost::HandleEvent(TX_CONSTHANDLE hAsyncData) {
	TX_HANDLE hEvent = TX_EMPTY_HANDLE;
	TX_HANDLE hBehavior = TX_EMPTY_HANDLE;

	txGetAsyncDataContent(hAsyncData, &hEvent);

	if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK) {
		OnGazeDataEvent(hBehavior);
		txReleaseObject(&hBehavior);
	}

	// NOTE since this is a very simple application with a single interactor and a single data stream, 
	// our event handling code can be very simple too. A more complex application would typically have to 
	// check for multiple behaviors and route events based on interactor IDs.
	txReleaseObject(&hEvent);
}

bool eyeXHost::RegisterEventHandler() {
	auto eventHandlerTrampoline = [](TX_CONSTHANDLE hObject, TX_USERPARAM userParam) {
		static_cast<eyeXHost*>(userParam)->HandleEvent(hObject);
	};

	bool success = txRegisterEventHandler(_context, &_eventHandlerTicket, eventHandlerTrampoline, this) == TX_RESULT_OK;
	return success;
}

eyeXHost::eyeXHost() {

}

eyeXHost::~eyeXHost() {
	if (_context != TX_EMPTY_HANDLE) {
		txDisableConnection(_context);
		txReleaseObject(&g_hGlobalInteractorSnapshot);
		txShutdownContext(_context, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE);
		txReleaseContext(&_context);
	}
}


void eyeXHost::init() {

	_context = TX_EMPTY_HANDLE;
    _connectionStateChangedTicket = TX_INVALID_TICKET;
    _eventHandlerTicket = TX_INVALID_TICKET;
	bool success;

    success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
    success &= txCreateContext(&_context, TX_FALSE) == TX_RESULT_OK;
    success &= InitializeGlobalInteractorSnapshot(_context);
	RegisterConnectionStateChangedHandler();
	RegisterEventHandler();
	success &= txEnableConnection(_context) == TX_RESULT_OK;
}
