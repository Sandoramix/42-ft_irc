#include <sstream>
#include "ResponseMsg.hpp"

// PRIVATE METHODS ------------------------------------------------------------
std::string ResponseMsg::hostname;

ResponseMsg::ResponseMsg() { }
ResponseMsg::~ResponseMsg() { }

std::string ResponseMsg::getDefaultMessage(ResponseCode code)
{
	switch (code) {
	case RPL_WELCOME:
		return "Welcome to the IRC Server @ " + hostname;
	case ERR_UNKNOWNCOMMAND:
		return "Unknown command";
	case ERR_NEEDMOREPARAMS:
		return "Not enough parameters";
	case ERR_ALREADYREGISTRED:
		return "You may not reregister";
	case ERR_PASSWDMISMATCH:
		return "Password incorrect";
	default:
		return "Unknown error";
	}
}



// PUBLIC METHODS -------------------------------------------------------------

void ResponseMsg::setHostname(const std::string& newHostname)
{ hostname = newHostname; }

bool ResponseMsg::isHostnameSet()
{
	if (!ResponseMsg::hostname.empty()) {
		return true;
	}
	std::cerr << "Error: hostname is empty" << std::endl;
	return false;
}

std::string ResponseMsg::genericResponse(ResponseCode code, const std::string& target)
{
	return genericResponse(code, target, getDefaultMessage(code));
}

std::string ResponseMsg::genericResponse(ResponseCode code, const std::string& target, const std::string& customMessage)
{
	std::string host = isHostnameSet() ? hostname : "*";
	std::stringstream ss;

	std::stringstream codeStream;
	codeStream << code;
	int codeSize = codeStream.str().size();
	codeStream.str("");
	while (codeStream.str().size() + codeSize < 3) {
		codeStream << "0";
	}
	codeStream << code;

	std::string codeAsString = codeStream.str();

	ss << ":" << host << " " << codeAsString << " " << (target.empty() ? "*" : target) << " :" << customMessage;
	return ss.str();
}
