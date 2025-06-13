#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <curl/curl.h>
#include <json/json.h>
using namespace std;


size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t newLength = size * nmemb;
    try {
        s -> append((char*)contents, newLength);
    } catch (std::bad_alloc &e) {
        cerr << "Error allocating memory in WriteCallback: " << e.what() << "\n";
        return 0;
    }
    return newLength;
}

string curl_GET(string base_url, string auth_header) {
    CURL *curl;
    CURLcode res;

    string result;

    curl = curl_easy_init();

    if (curl) {
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, auth_header.c_str());
         
	curl_easy_setopt(curl, CURLOPT_URL, base_url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);	
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

	res = curl_easy_perform(curl);

	curl_easy_cleanup(curl);
	return result;
    }

    return "Error: curl was not initialized properly";
}

map<string, string> latest_assignments(Json::Value assignment_data) {
    map<string, string> assignments;

    for (const auto& assignment: assignment_data) {
	const auto now = chrono::system_clock::now();
	const time_t time_now = chrono::system_clock::to_time_t(now);

        string due_date_str = assignment.get("due_at", "NO DUE DATE GIVEN").asString();
	string assignment_name = assignment.get("name", "ASSIGNMENT UNNAMED").asString();

	tm due_date{};
	istringstream ss(due_date_str);

    	const char* format = "%Y-%m-%dT%H:%M%S";
    	ss >> get_time(&due_date, format);

	time_t due_at = timegm(&due_date);
	tm* due_local = localtime_r(&due_at, &due_date);

	string months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	string weekdays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

	due_date_str.replace(
	    due_date_str.begin(), due_date_str.end(), 
	    weekdays[due_local -> tm_wday] + ", " + months[due_local -> tm_mon] + " " + to_string(due_local -> tm_mday)  + " " + to_string(1900 + due_local -> tm_year) + 
	    " " + to_string(due_local -> tm_hour) + ":" + to_string(due_local -> tm_min)
	);
	
		if (due_at > time_now) {
	            assignments[assignment_name] = due_date_str;
		}
    }

    return assignments;
}

int main() {
    string base_url = "https://ucsc.instructure.com/api/v1/courses/84010/assignments";
    string auth_header = "Authorization: Bearer 9270~6CALRYUF9uCXJZBwYy8zELKrJwfy3cXvUhPCyJyDUUWe4ER6YuLLULPBcLeXwQam";
    string result;

    ofstream log_file("/tmp/canvas_debug.log", ios_base::app);
    log_file << "Script started at: " << time(nullptr) << "\n";

    Json::CharReaderBuilder readerBuilder;
    Json::Value jsonData;
    unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
    string errs;

    result = curl_GET(base_url, auth_header);
    bool is_parsed = reader -> parse(result.c_str(), result.c_str() + result.length(), &jsonData, &errs);
    map<string, string> current_assignments = latest_assignments(jsonData);
	/*
    std::string yuck_output_string = R"x((box :orientation "vertical" :space-evenly false :spacing 5)x" + std::string("\n");
    yuck_output_string += "    " + std::string(R"x((label :width 100 :text "Cultures of India Assignments: "))x") + "\n";
    for (auto const& [name, due_date] : current_assignments) {
        yuck_output_string += "        " + std::string(R"x((label :width 100 :text "Name - )x") + name + R"x("))x" + "\n";
        yuck_output_string += "        " + std::string(R"x((label :width 100 :text "Due At - )x") + due_date + R"x("))x" + "\n\n";
    }
    yuck_output_string += ")";

    std::cout << yuck_output_string << std::endl; // Still print to stdout
    log_file << "Canvas script YUCK OUTPUT:\n" << yuck_output_string << std::endl;
    log_file << "Canvas script finished at: " << std::time(nullptr) << std::endl;
    log_file.close();
	*/
    cout << R"x((box :orientation "vertical" :space-evenly false :spacing 5)x" << flush; 
    cout << R"x((label :width 100 :text "Cultures of India Assignments: ")x" << ")" << flush;
    for (auto assignment: current_assignments) {
        cout << R"x((label :width 100 :text "Name - )x" << assignment.first << R"x("))x" << flush;
        cout << R"((label :width 100 :text "Due At - )" << assignment.second << R"x("))x" << flush;
    }
    cout << ")" << flush;

    return 0;
}
