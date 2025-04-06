#include <iostream>
#include <string>
#include "jstream.h"
#include "strformat.h"

std::string getline()
{
	std::string line;
	std::getline(std::cin, line);
	return line;
}

struct Message {
	std::string method;
	struct Params {
		std::string protocolversion;
		bool capabilities_roots_listchanged = false;
		bool capabilities_prompts_listchanged = false;
		bool capabilities_resources_subscribe = false;
		bool capabilities_resources_listchanged = false;
		std::string name;
		struct Argument {
			std::string name;
			std::string value;
		};
		std::vector<Argument> arguments;
	} params;
	struct Result {
		std::string protocolversion;
		bool capabilities_prompts_listchanged = false;
		bool capabilities_resources_subscribe = false;
		bool capabilities_resources_listchanged = false;
		bool capabilities_tools_listchanged = false;
		std::string serverinfo_name;
		std::string serverinfo_version;
		struct Tool {
			std::string name;
			std::string description;
			struct InputSchema {
				struct Property {
					std::string name;
					std::string title;
					std::string type;
				};
				std::vector<Property> properties;
				std::vector<std::string> required;
				std::string title;
				std::string type;
			} input_schema;
		};
		std::vector<Tool> tools;
	} result;
	std::string params_clientinfo_name;
	std::string params_clientinfo_version;
	std::string jsonrpc;
	std::string id;
};

Message parse_message(std::string const &line)
{
	char const *begin = line.c_str();
	char const *end = begin + line.size();
	Message request;
	Message::Result::Tool tool;
	Message::Result::Tool::InputSchema::Property property;

	jstream::Reader reader(begin, end);
	while (reader.next()) {
		if (reader.match("{method")) {
			request.method = reader.string();
		} else if (reader.match("{params{protocolVersion")) {
			request.params.protocolversion = reader.string();
		} else if (reader.match("{params{capabilities{roots{listChanged")) {
			request.params.capabilities_roots_listchanged = reader.istrue();
		} else if (reader.match("{params{clientInfo{name")) {
			request.params_clientinfo_name = reader.string();
		} else if (reader.match("{params{clientInfo{version")) {
			request.params_clientinfo_version = reader.string();
		} else if (reader.match("{params{name")) {
			request.params.name = reader.string();
		} else if (reader.match("{params{arguments{*")) {
			if (reader.isvalue()) {
				Message::Params::Argument a;
				a.name = reader.key();
				a.value = reader.string();
				request.params.arguments.push_back(a);
			}
		} else if (reader.match("{result{protocolVersion")) {
			request.result.protocolversion = reader.string();
		} else if (reader.match("{result{capabilities{prompts{listChanged")) {
			request.result.capabilities_prompts_listchanged = reader.istrue();
		} else if (reader.match("{result{capabilities{resources{subscribe")) {
			request.result.capabilities_resources_subscribe = reader.istrue();
		} else if (reader.match("{result{capabilities{resources{listChanged")) {
			request.result.capabilities_resources_listchanged = reader.istrue();
		} else if (reader.match("{result{capabilities{tools{listChanged")) {
			request.result.capabilities_tools_listchanged = reader.istrue();
		} else if (reader.match("{result{serverInfo{name")) {
			request.result.serverinfo_name = reader.string();
		} else if (reader.match("{result{serverInfo{version")) {
			request.result.serverinfo_version = reader.string();
		} else if (reader.match("{result{tools[{name")) {
			tool.name = reader.string();
		} else if (reader.match("{result{tools[{description")) {
			tool.description = reader.string();
		} else if (reader.match("{result{tools[{inputSchema{properties{*{title")) {
			property.title = reader.string();
		} else if (reader.match("{result{tools[{inputSchema{properties{*{type")) {
			property.type = reader.string();
		} else if (reader.match("{result{tools[{inputSchema{properties{*")) {
			if (reader.state() == jstream::StartObject) {
				property.name = reader.key();
			} else if (reader.state() == jstream::EndObject) {
				tool.input_schema.properties.push_back(property);
				property = {};
			}
		} else if (reader.match("{result{tools[{inputSchema{required[*")) {
			tool.input_schema.required.push_back(reader.string());
		} else if (reader.match("{result{tools[{inputSchema{title")) {
			tool.input_schema.title = reader.string();
		} else if (reader.match("{result{tools[{inputSchema{type")) {
			tool.input_schema.type = reader.string();
		} else if (reader.match("{jsonrpc")) {
			request.jsonrpc = reader.string();
		} else if (reader.match("{id")) {
			request.id = reader.string();
		}
		if (reader.state() == jstream::StartObject) {
			if (reader.path() == "{result{tools[{") {
				tool = {};
			} else if (reader.path() == "{result{tools[{inputSchema{properties{") {
				property = {};
			}
		} else if (reader.state() == jstream::EndObject) {
			if (reader.path() == "{result{tools[") {
				request.result.tools.push_back(tool);
				tool = {};
			}
		}
	}
	return request;
}

int main()
{
	if (0) { // for debug

		if (1) {
			std::string line = R"---({"method": "initialize","params": {"protocolVersion": "2024-11-05","capabilities": {"roots": {"listChanged": true}},"clientInfo": {"name": "mcp","version": "0.1.0"}},"jsonrpc": "2.0","id": 0}})---";
			Message request = parse_message(line);
			return 0;
		}
		if (0) {
			std::string line = R"---({"jsonrpc": "2.0","id": 0,"result": {"protocolVersion": "2024-11-05","capabilities": {"experimental": {},"prompts": {"listChanged": false},"resources": {"subscribe": false,"listChanged": false},"tools": {"listChanged": false}},"serverInfo": {"name": "Demo","version": "1.6.0"}}})---";
			Message request = parse_message(line);
			return 0;
		}
		if (0) {
			std::string line = R"---({"method": "notifications/initialized","jsonrpc": "2.0","params": null})---";
			Message request = parse_message(line);
			return 0;
		}
		if (0) {
			std::string line = R"---({"method": "tools/list","jsonrpc": "2.0","id": 1})---";
			Message request = parse_message(line);
			return 0;
		}
		if (0) {
			std::string line = R"---({"jsonrpc": "2.0","id": 1,"result": {"tools": [{"name": "add","description": "Add two numbers","inputSchema": {"properties": {"a": {"title": "A","type": "integer"},"b": {"title": "B","type": "integer"}},"required": ["a","b"],"title": "addArguments","type": "object"}},{"name": "light_switch","description": "Turn on or turn off the light","inputSchema": {"properties": {"on": {"title": "On","type": "boolean"}},"required": ["on"],"title": "light_switchArguments","type": "object"}}]}})---";
			Message request = parse_message(line);
			return 0;
		}
		if (0) {
			std::string line = R"---({"method":"tools/call","params":{"name":"add","arguments":{"a":3,"b":4}},"jsonrpc":"2.0","id":25})---";
			Message request = parse_message(line);
			return 0;
		}
		return 0;
	}

	while (1) {
		std::string line = getline();

		Message request = parse_message(line);
		if (request.method == "initialize") {
			std::vector<char> vec;
			jstream::Writer writer([](char const *p, int n){
				fwrite(p, 1, n, stdout);
			});
			writer.enableIndent(false);
			writer.enableNewLine(false);
			writer.object({}, [&](){
				writer.string("jsonrpc", "2.0");
				writer.number("id", atoi(request.id.c_str()));
				writer.object("result", [&](){
					writer.string("protocolVersion", "2024-11-05");
					writer.object("capabilities", [&](){
						writer.object("experimental", [&](){});
						writer.object("prompts", [&](){
							writer.boolean("listChanged", false);
						});
						writer.object("resources", [&](){
							writer.boolean("subscribe", false);
							writer.boolean("listChanged", false);
						});
						writer.object("tools", [&](){
							writer.boolean("listChanged", false);
						});
					});
					writer.object("serverInfo", [&](){
						writer.string("name", "Demo");
						writer.string("version", "1.6.0");
					});
				});
			});
			std::string_view view(vec.data(), vec.size());
			puts(std::string(view).c_str());
			continue;
		}
		if (request.method == "notifications/initialized") {
			continue;
		}
		if (request.method == "tools/list") {
			std::vector<char> vec;
			jstream::Writer writer([&](char const *p, int n){
				vec.insert(vec.end(), p, p + n);
			});
			writer.enableIndent(false);
			writer.enableNewLine(false);
			writer.object({}, [&](){
				writer.string("jsonrpc", "2.0");
				writer.number("id", atoi(request.id.c_str()));
				writer.object("result", [&](){
					writer.array("tools", [&](){
						writer.object({}, [&](){
							writer.string("name", "add");
							writer.string("description", "Add two numbers");
							writer.object("inputSchema", [&](){
								writer.object("properties", [&](){
									writer.object("a", [&](){
										writer.string("title", "A");
										writer.string("type", "integer");
									});
									writer.object("b", [&](){
										writer.string("title", "B");
										writer.string("type", "integer");
									});
								});
								writer.array("required", [&](){
									writer.string("a");
									writer.string("b");
								});
								writer.string("title", "addArguments");
								writer.string("type", "object");
							});
						});
					});
				});
			});
			std::string_view view(vec.data(), vec.size());
			puts(std::string(view).c_str());
			continue;
		}
		if (request.method == "resources/list") {
			std::vector<char> vec;
			jstream::Writer writer([&](char const *p, int n){
				vec.insert(vec.end(), p, p + n);
			});
			writer.enableIndent(false);
			writer.enableNewLine(false);
			writer.object({}, [&](){
				writer.string("jsonrpc", "2.0");
				writer.number("id", atoi(request.id.c_str()));
				writer.object("result", [&](){
				});
			});
			std::string_view view(vec.data(), vec.size());
			puts(std::string(view).c_str());
			continue;
		}
		if (request.method == "prompts/list") {
			std::vector<char> vec;
			jstream::Writer writer([&](char const *p, int n){
				vec.insert(vec.end(), p, p + n);
			});
			writer.enableIndent(false);
			writer.enableNewLine(false);
			writer.object({}, [&](){
				writer.string("jsonrpc", "2.0");
				writer.number("id", atoi(request.id.c_str()));
				writer.object("result", [&](){
				});
			});
			std::string_view view(vec.data(), vec.size());
			puts(std::string(view).c_str());
			continue;
		}
		if (request.method == "tools/call") {
			// return example:
			// {"jsonrpc":"2.0","id":21,"result":{"content":[{"type":"text","text":"13"}],"isError":false}}
			if (request.params.name == "add") {
				if (request.params.arguments.size() == 2) {
					std::vector<char> vec;
					jstream::Writer writer([&](char const *p, int n){
						vec.insert(vec.end(), p, p + n);
					});
					writer.enableIndent(false);
					writer.enableNewLine(false);
					writer.object({}, [&](){
						writer.string("jsonrpc", "2.0");
						writer.number("id", atoi(request.id.c_str()));
						writer.object("result", [&](){
							writer.array("content", [&](){
								writer.object({}, [&](){
									int a = atoi(request.params.arguments[0].value.c_str());
									int b = atoi(request.params.arguments[1].value.c_str());
									int ans = a + b;
									writer.string("type", "text");
									writer.string("text", strformat("%d")(ans));
								});

							});
							writer.boolean("isError", false);
						});
					});
					std::string_view view(vec.data(), vec.size());
					puts(std::string(view).c_str());
					continue;
				}
			}
		}
	}

	return 0;
}
