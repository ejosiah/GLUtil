#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>


namespace ncl {

	static const unsigned int _trace = 0x01;
	static const unsigned int _debug = 0x02;
	static const unsigned int _info =  0x04;
	static const unsigned int _warn = 0x08;
	static const unsigned int _error = 0x10;

	const unsigned int LEVEL_ERROR = 0 | _error;
	const unsigned int LEVEL_WARN = LEVEL_ERROR | _warn;
	const unsigned int LEVEL_INFO = LEVEL_WARN | _info;
	const unsigned int LEVEL_DEBUG = LEVEL_INFO | _debug;
	const unsigned int LEVEL_TRACE = LEVEL_DEBUG | _trace;

	class Appender{
	public:
		virtual void append(std::string& msg) = 0;
	};

	class OstreamAppender : public Appender{
	public:
		OstreamAppender(std::ostream& out) :out(out) {

		}

		virtual ~OstreamAppender() = default;

		virtual void append(std::string& msg) override{
			out << msg << std::endl;
		}

	protected:
		std::ostream& out;
	};

	class ConsoleAppender : public OstreamAppender{
	public:
		ConsoleAppender()
		:OstreamAppender(std::cout){
		}
	};

	class FileAppender : public OstreamAppender{
	public:
		FileAppender(std::string path)
		:OstreamAppender(fout){
			fout.open(path, std::ios_base::app);
			if (!fout) {
				throw std::runtime_error("unable to open log file");
			}
		}

		virtual ~FileAppender() {
			fout.flush();
			fout.close();
		}

	private:
		std::ofstream fout;
	};

	class Logger {
	public:
		Logger() {}

		Logger(std::string owner) {
			this->owner = owner;
		}

		void debug(std::string msg) {
			std::string augMsg = "[DEBUG] [" + owner + "]: " + msg;
			if(level & _debug){
				append(augMsg);
			}
		}
		void info(std::string msg) {
			std::string augMsg = "[INFO] [" + owner + "]: " + msg;
			if(level & _info){
				append(augMsg);
			}
		}

		void warn(std::string msg) {
			std::string augMsg = "[WARN] [" + owner + "]: " + msg;
			if(level & _warn){
				append(augMsg);
			}
		}

		void error(std::string msg, std::runtime_error& err) {
			std::string augMsg = "[ERROR] [" + owner + "]: " + msg + ": " + err.what();
			if(level & _error){
				append(augMsg);
			}

		}

		void error(std::string msg) {
			std::string augMsg = "[ERROR] [" + owner + "]: " + msg;
			if (level & _error) {
				append(augMsg);
			}

		}

		static Logger& get(std::string owner) {
			auto itr = loggers.find(owner);
			if (itr == loggers.end()) {
				loggers[owner] = Logger{ owner };
			}
			return loggers[owner];
		}

		void append(std::string msg) {
			for (Appender* appender : appenders) {
				appender->append(msg);
			}
		}

	private:
		std::string owner;
		static std::map<std::string, Logger> loggers;
		static std::vector<Appender*> appenders;
		static const unsigned int level;
	};

	std::map<std::string, Logger> Logger::loggers;
}