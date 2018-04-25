//
//#include "RelayServer.h"
//#include "UDPServerProtocol.h"
//
//namespace RelayServerHelper {
//	void postpendInt(ServerData& data, unsigned int number) {
//		data.push_back(number >> 24);
//		data.push_back((number >> 16) & 255);
//		data.push_back((number >> 8) & 255);
//		data.push_back(number & 255);
//	}
//
//	unsigned int getPostpendedInt(ServerData& data, unsigned int fromEnd) {
//		int startpos = data.size() - fromEnd-1;
//		if (startpos < 3) return 0;
//		unsigned int res = (unsigned int)data[startpos] | (((unsigned int)data[startpos - 1]) << 8)
//			| (((unsigned int)data[startpos - 2]) << 16) | (((unsigned int)data[startpos - 3]) << 24);
//		return res;
//	}
//}
//
//using namespace RelayServerHelper;
//
//bool RelayServer::handleMessage(ServerData& data, Connection& from)
//{
//	if (data.empty()) {
//		reportCheat(data, from);
//		return false;
//	}
//	UDPMessageType s = static_cast<UDPMessageType>( *data.rbegin() );
//	bool cheat = false;
//	switch (s){
//	case SHUT_DOWN:
//		if(from.getPermissions() >=  from.LOGIN_SERVER) return true;
//		cheat = true;
//		break;
//	case CONNECT_LOGIN:
//		if (from.getPermissions() >= from.LOGIN_SERVER) {
//			if (loginServer) {
//				delete loginServer;
//			}
//			sendLoginRequest(data);
//			return false;
//		}
//		cheat = true;
//		break;
//	case CONNECT_SERVER:
//		if (from.getPermissions == from.LOGIN_SERVER) {
//			connectServer(data);
//			return false;
//		}
//		cheat = true;
//		break;
//	case CONNECT_CLIENT:
//		if (from.getPermissions == from.LOGIN_SERVER) {
//			connectClient(data);
//			return false;
//		}
//	case REQUEST_DO_ACTIONS:
//		if ( from.testPermission( from.MASTER ) ) {
//			if (requestDoActions(data)) return false;
//
//			/*
//			ServerStepCount step = getPostpendedInt(data, 1);
//			if (step <= lastSendtUp) {
//				sendTooLate(from, step);
//			}
//			if (lastSendtUp > 0 && step > lastSendtUp + 5) {
//				cheat = true;
//				break;
//			}
//			upStream[step].insert(upStream[step].end(), data[0], data[data.size() - 5]);
//			return false;
//			*/
//		}
//		cheat = true;
//		break;
//	case CONFIRMED_ACTIONS:
//		if (from.getPermissions() == from.SERVER) {
//			confirmedActions(data);
//			/*
//			ServerStepCount step = getPostpendedInt(data, 1);
//			if (downStreamOffset == 0) downStreamOffset = step;
//			int pos = step - downStreamOffset;
//			if (pos < 0) return false;
//			if (pos == downStream.size()) {
//				downStream.emplace_back(data[0], data[data.size() - 5]);
//				sendUp(pos + STEP_DELAY_MARGINS);
//			} else if (pos<downStream.size()) {
//				if (downStream[pos].size() == 0) {
//					downStream[pos].insert(downStream[pos].end(), data[0], data[data.size() - 5]);
//				}
//				else {
//					return false;
//				}
//			}
//			else if (pos < downStream.size() + RECOVER_LIMIT) {
//				requestResendActions( downStream.size() + downStreamOffset, pos + downStreamOffset - 1);
//				downStream.resize(pos);
//				downStream.emplace_back(data[0], data[data.size() - 5]);
//				sendUp(pos + STEP_DELAY_MARGINS);
//			}
//			else {
//				// Lost connection for too long to recover
//				return true;
//			}
//			sendDownstream(pos);
//			return false;
//			*/
//		}
//
//		cheat = true;
//		break;
//	case RESEND_ACTIONS:
//		if (from.getPermissions() == from.CLIENT || from.getPermissions() == from.MASTER) {
//			resendActions(from, data);
//			/*
//			ServerStepCount stepStart = getPostpendedInt(data, 1);
//			ServerStepCount stepEnd = getPostpendedInt(data, 5);
//			int pos = stepStart - downStreamOffset;
//			if (pos < 0 ) { 
//				sendNewServer(from);
//			}
//			else {
//				int len = stepEnd - stepStart + 1;
//				for (int i = 0; i < len; i++) {
//					if (pos + i >= downStream.size()) {
//						break;
//					}
//					if (downStream[pos+i].size() == 0 && server) {
//						requestResendActions(pos + i + downStreamOffset, pos + i + downStreamOffset);
//					}
//					else {
//						sendDownstream(pos + i);
//					}
//				}
//			}
//			*/
//			return false;
//			
//		}
//		cheat = true;
//		break;
//	case DO_TICK:
//		if (from.getPermissions == from.LOGIN_SERVER && !server ) {
//			/*
//			ServerStepCount step = getPostpendedInt(data, 1);
//			while (upStream.begin() != upStream.end() && upStream.begin()->first < step) {
//				upStream.erase(upStream.begin());
//			}
//			if (downStream.size() + downStreamOffset > step) {
//				cheat = true;
//				break;
//			}
//			if (downStreamOffset == 0) {
//				downStreamOffset = step;
//			}
//			int pos = step - downStreamOffset;
//			if (upStream.begin() == upStream.end() || upStream.begin()->first > step) {
//
//				downStream.resize(pos);
//				downStream.emplace_back();
//			}
//			else {
//				downStream.resize(pos);
//				downStream.emplace_back(upStream.begin()->second);
//			}
//			sendDownStream(pos);
//			*/
//		}
//		cheat = true;
//		break;
//	case NAT_BREAK:
//		natBreak(data);
//		return false;
//	default:
//		cheat = true;
//	}
//	if (cheat) {
//		//cheathandeling code here;
//		reportCheat(data, from);
//	}
//
//	return false;
//}
//
