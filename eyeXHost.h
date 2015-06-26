#pragma once

#include "ofMain.h"
#include "stdafx.h"
#include "eyex/EyeX.h"
#include <vector>
#include <mutex>

class eyeXHost {
	public:

		eyeXHost();
		virtual ~eyeXHost();

		void init();
		
		ofVec2f gazePoint;

			TX_CONTEXTHANDLE _context;
			TX_TICKET _connectionStateChangedTicket;
			TX_TICKET _eventHandlerTicket;

			bool InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext);
			void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState);
			void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData);
			bool RegisterConnectionStateChangedHandler();
			bool RegisterEventHandler();
		    
			void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData);
			void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior);
			void HandleQuery(TX_CONSTHANDLE hAsyncData);
			void HandleActivatableEvent(TX_HANDLE hEvent, int interactorId);
			void OnActivationFocusChanged(TX_HANDLE hBehavior, int interactorId);
			void OnActivated(TX_HANDLE hBehavior, int interactorId);

			static bool QueryIsForWindowId(TX_HANDLE hQuery, const TX_CHAR* windowId);
};
