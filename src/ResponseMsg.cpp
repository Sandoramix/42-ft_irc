#include <sstream>
#include "ResponseMsg.hpp"

// PRIVATE METHODS ------------------------------------------------------------
std::string ResponseMsg::hostname;

ResponseMsg::ResponseMsg() { }
ResponseMsg::~ResponseMsg() { }

std::string ResponseMsg::getDefaultMessage(ResponseCode code)
{
	switch (code) {
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

std::string ResponseMsg::errorResponse(ResponseCode code, const std::string& target)
{
	if (!isHostnameSet()) {
		return std::string();
	}
	std::stringstream ss;
	ss << ":" << hostname << " " << code << " " << (target.empty() ? "*" : target) << " " << getDefaultMessage(code);
	return ss.str();
}

std::string ResponseMsg::errorResponse(ResponseCode code, const std::string& target, const std::string& message)
{
	if (!isHostnameSet()) {
		return std::string();
	}
	std::stringstream ss;
	ss << ":" << hostname << " " << code << " " << (target.empty() ? "*" : target) << " " << message;
	return ss.str();
}
