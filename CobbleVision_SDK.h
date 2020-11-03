#include <stdbool.h>
#include <cjson/cJSON.h> 

// Functions are all sync. This is due to C requirement to create seperate processes and use other threads for async functions.

// #################################################
// Preparation of Variables and Environment Settings.
// #################################################

bool environmentType = false;
char valid_price_categories=["high", "medium", "low"]
char valid_job_types=["QueuedJob"]

bool debugging = false
char serverAddress = "https://www.cobblevision.com"

char apiUsername = ""
char apiToken = ""

if (environmentType==false || environmentType === "demo"){
  char BaseURL = "https://www.cobblevision.com"
}else{
  char BaseURL = serverAddress + "/api/"
}

// Function allows you to set the Username and Token for CobbleVision
// @function setApiAuth()// @param {String} apiusername
// @param {String} apitoken
// @returns {Boolean} Indicating success of setting Api Auth.

extern 'C' bool setAPIAuth(char ApiUsername, char ApiToken){
  apiUsername = ApiUsername
  apiToken = ApiToken
  return true
}

 // # Function allows you to set the debugging variable
 // # @function setDebugging()
 // # @param {Boolean} debugBool
 // # @returns {Boolean} Indicating success of setting Api Auth.
extern 'C' bool setDebugging(bool debugBool){
  debugging = debugBool
  return true
}

// #####################################################
// # Functions for interacting with the CobbleVision API
// #####################################################

// # Return of the following functions is specified within this type description
// # @typedef {Object} CompletableFuture <HTTPEntity> Entity from URLConnection as Completable Future
// # @method {String} response contains response Data as string


// # This function uploads a media file to CobbleVision. You can find it after login in your media storage. Returns a response object with body, response and headers properties, deducted from npm request module
// # @sync
// # @function uploadMediaFile()  
// # @param {string} price_category - Either high, medium, low
// # @param {boolean} publicBool - Make Media available publicly or not?
// # @param {string} name - Name of Media (Non Unique)
// # @param {array} tags - Tag Names for Media - Array of Strings
// # @param {BufferedImage} file - fileBuffer from file.read
// # @returns {Response} This return the UploadMediaResponse as HttpEntity. The body is in JSON format.
extern 'C' char uploadMediaFile(char price_category, bool publicBool, char name, char tags[], char file[]){
  char endpoint = "media"
  
  if(BaseURL.charAt(sizeOf(BaseURL)-1) != "/"){
    printf("BaseURL must end with slash");
    exit(0)
  }
  
  //Verify the strchar function on char []. usually used on chars, not char [].
  if(!strchar(valid_price_categories,price_category)){
    printf("Job Type is not valid!")
    exit(0)
  }
  
  //According to Web ressources file type checking in c runtime are not possible
  
  cJSON jsonObject = CJSON_CreateObject()
  cJSON_AddStringToObject(jsonObject, "price_Category", price_category)
  cJSON_AddBoolToObject(jsonObject, "publicBool", publicBool)
  cJSON_AddStringToObject(jsonObject, "name", name)
  cJSON_AddArrayToObject(jsonObject, "tags", tags)
  cJSON.AddStringToObject(jsonObject, "file", file)
  
  char response = sendRequestToAPI("POST", jsonObject, BaseURL + endpoint)
  return response
}

// # This function deletes Media from CobbleVision 
// # @sync
// # @function deleteMediaFile()  
// # @param {array} IDArray Array of ID's as Strings
// # @returns {Response} This return the DeleteMediaResponse. The body is in JSON format.
extern 'C' char deleteMediaFile(char IDArray[]){
  char endpoint = "media"
  
  if(BaseURL.charAt(sizeOf(BaseURL)-1) != "/"){
    printf("BaseURL must end with slash");
    exit(0)
  }
  
  bool validResult = checkIDArrayForInvalidIDs(IDArray)
  if(validResult == false){
    printf("Provided Media IDs which are not valid!")
    exit(0)
  }
  
  cJSONArray json_idArray = cJSON_Parse(IDArray)
  char response = sendRequestToAPI("DELETE", null, BaseURL + endpoint + "?id=" + cJSON_Print(json_idArray))
  
  return response
}

// # Launch a calculation with CobbleVision's Web API. Returns a response object with body, response and headers properties, deducted from npm request module;
// # @sync
// # @function launchCalculation() 
// # @param {array} algorithms Array of Algorithm Names
// # @param {array} media Array of Media ID's  
// # @param {string} type Type of Job - Currently Always "QueuedJob"
// # @param {string} [notificationURL] Optional - Notify user upon finishing calculation!
// # @returns {Response} This returns the LaunchCalculationResponse. The body is in JSON format.  

extern 'C' char launchCalculation(char algorithms[], char media[], char type, char notificationURL){
  char endpoint = "calculation"
  
  if(BaseURL.charAt(sizeOf(BaseURL)-1) != "/"){
    printf("BaseURL must end with slash");
    exit(0)
  }
  
  bool validMedia = checkIDArrayForInvalidIDs(media)
  if(validMedia == false){
    printf("Provided Media IDs which are not valid!")
    exit(0)
  }
  
  bool validAlgorithms = checkIDArrayForInvalidIDs(algorithms)
  if(validMedia == false){
    printf("Provided Media IDs which are not valid!")
    exit(0)
  }
  
  //Verify the strchar function on char []. usually used on chars, not char [].
  if(!strchar(valid_job_types,type)){
    printf("Job Type is not valid!")
    exit(0)
  }
  
  cJSON jsonObject = CJSON_CreateObject()
  cJSON_AddArrayToObject(jsonObject, "algorithms", algorithms)
  cJSON_AddArrayToObject(jsonObject, "media", media)
  cJSON_AddStringToObject(jsonObject, "type", type)
  cJSON_AddStringToObject(jsonObject, "notificationURL", notificationURL)
  
  char response = sendRequestToAPI("POST", jsonObject, BaseURL + endpoint)
  
  return response
}

// # This function waits until the given calculation ID's are ready to be downloaded!
// # @sync
// # @function waitForCalculationCompletion() 
// # @param {array} calculationIDArray Array of Calculation ID's
// # @returns {Response} This returns the WaitForCalculationResponse. The body is in JSON format.   

extern 'C' char waitForCalculationCompletion(char IDArray[]){
  char endpoint = "calculation"
  
  if(BaseURL.charAt(sizeOf(BaseURL)-1) != "/"){
    printf("BaseURL must end with slash");
    exit(0)
  }
  
  bool validResult = checkIDArrayForInvalidIDs(IDArray)
  if(validResult == false){
    printf("Provided Calculation IDs which are not valid!")
    exit(0)
  }
  
  cJSONArray json_idArray = cJSON_Parse(IDArray)
  bool calculationFinishedBool = false;
  while(!calculationFinishedBool(){  
    char response = sendRequestToAPI("GET", null, BaseURL + endpoint + "?id=" + cJSON_print(json_idArray), + "&returnOnlyStatusBool=true")
    cJSONArray responseArray = cJSON_Parse(response)
    if(cJSON_IsArray(responseArray)){
      for(int i=0; i<cJSON_GetArraySize(responseArray); i++){
        if(responseArray[i].status){
          if(responseArray[i].status != "finished"){
            calculationFinishedBool = false
          }
        }else{
          calculationFinishedBool = false;
          break;
        }
      }
    }else{
      responseJSON = cJSON_Parse(response)
      if(responseJSON.error){
        calculationFinishedBool = true
      }
    }
  }
  
  return response
}

// # This function deletes Result Files or calculations in status "waiting" from CobbleVision. You cannot delete finished jobs beyond their result files, as we keep them for billing purposes.
// # @sync
// # @function deleteCalculation()
// # @param {array} IDArray Array of ID's as Strings
// # @returns {Response} This returns the DeleteCalculationResponse. The body is in JSON format.
extern 'C' char deleteCalculation(char IDArray[]){
  char endpoint = "calculation"
  
  if(BaseURL.charAt(sizeOf(BaseURL)-1) != "/"){
    printf("BaseURL must end with slash");
    exit(0)
  }
  
  bool validResult = checkIDArrayForInvalidIDs(IDArray)
  if(validResult == false){
    printf("Provided Calculation IDs which are not valid!")
    exit(0)
  }
  
  cJSONArray json_idArray = cJSON_Parse(IDArray)
  char response = sendRequestToAPI("DELETE", null, BaseURL + endpoint + "?id=" + cJSON_Print(json_idArray))
  
  return response
}

// # Get Calculation Result with CobbleVision's Web API. Returns a response object with body, response and headers properties, deducted from npm request module;
// # @sync
// # @function getCalculationResult()
// # @param {array} IDArray ID of calculation to return result Array 
// # @param {boolean} returnOnlyStatusBool Return full result or only status? See Doc for more detailed description!
// # @returns {Response} This returns the GetCalculationResult. The body is in json format.
extern 'C' char getCalculationResult(char IDArray[], bool returnOnlyStatusBool){
  char endpoint = "calculation"
  
  if(BaseURL.charAt(sizeOf(BaseURL)-1) != "/"){
    printf("BaseURL must end with slash");
    exit(0)
  }
  
  bool validResult = checkIDArrayForInvalidIDs(IDArray)
  if(validResult == false){
    printf("Provided Calculation IDs which are not valid!")
    exit(0)
  }
  
  cJSONArray json_idArray = cJSON_Parse(IDArray)
  char response = sendRequestToAPI("GET", null, BaseURL + endpoint + "?id=" + cJSON_Print(json_idArray) + "&returnOnlyStatusBool=" + (char) returnOnlyStatusBool)
  
  return response
}

// # Request your calculation result by ID with the CobbleVision API. Returns a response object with body, response and headers properties, deducted from npm request module;
// # @sync
// # @function getCalculationVisualization()
// # @param {string} id ID of calculation to return result/check String
// # @param {boolean} returnBase64Bool Return Base64 String or image buffer as string?
// # @param {integer} width target width of visualization file
// # @param {integer} height target height of visualization file
// # @returns {Response} This returns the GetCalculationVisualization Result. The body is in binary format.
extern 'C' char getCalculationVisualization(char IDArray[], bool returnBase64Bool){
  char endpoint = "calculation/visualization"
  
  if(BaseURL.charAt(sizeOf(BaseURL)-1) != "/"){
    printf("BaseURL must end with slash");
    exit(0)
  }
  
  bool validResult = checkIDArrayForInvalidIDs(IDArray)
  if(validResult == false){
    printf("Provided Calculation IDs which are not valid!")
    exit(0)
  }
  
  if(width==0){
    printf("Width cannot be zero!")
    exit(0)
  }
  
  if(height==0){
    printf("Height cannot be zero!")
    exit(0)
  }
  
  cJSONArray json_idArray = cJSON_Parse(IDArray)
  char response = sendRequestToAPI("GET", null, BaseURL + endpoint + "?id=" + cJSON_Print(json_idArray) + "&returnBase64Bool=" + (char) returnBase64Bool)
  
  return response
}

// Check Array of Mongo IDs for Invalid Values
// @sync
// @function checkIDArraysForInvalidIDs()
// @param {array} IDArray Array of Mongo IDs
// @returns {boolean} Success of Check
extern 'C' bool checkIDArraysForInvalidIDs(char IDArray[]){
  for(int i=0; i< sizeof(IDArray); i++){
    ObjectId(IDArray[i])
  }
  return true
}

// @sync
// @function wait()
// @param {number} ms time to wait in ms
// @returns {boolean} Success of Wait
extern 'C' bool wait(int ms){
  sleep(ms/1000)
  return true
}


// #####################################################
// # Web Requests in C++ are a little bit unusual
// #####################################################

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

// Callback Function for CURLOPT_WRITEFUNCTION
// @sync
// @function callback()
// @param {char} char in stream values
// @param {size_t} size Memory Size
// @param {size_t} num Number of elements
// @param {string} out Response values
// @returns {size_t} Memory Size
 std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out){
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}
    
// Functions provides interface for web requests
// @sync
// @function sendRequestToAPI()
// @param {char} type Type of Requests
// @param {cJSON_Object} jsonObject JSON to send with Requests
// @param {char} tUrl Target URL of Requests
// @returns {boolean} Success of Wait
char sendRequestToAPI(char type, cJSON jsonObject, char tUrl){
 const std:string url(tUrl)
 CURL* curl = curl.easy_init()
 
 curl_easy_setopt(webHandler, CURLOPT_CUSTOMREQUEST, type)
 curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
 curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
 curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
 struct urlList headers = NULL

 headers=curl_slist_append(headers, "Content_Type":"application/json")
 headers_curl_slist_append(headers, "Accept", "application/json")
 curl_easy_setopt(webHandler, CURLOPT_HTTPHEADER, headers)
  
 long httpCode(0);
 std::unique_ptr<std::string> httpData(new std::string());

 // Hook up data handling function.
 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
 curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

 curl_easy_perform(curl);
 curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
 curl_easy_cleanup(curl);
 
 if(httpCode != 200) {
   fprintf(stderr, "curl_easy_perform() failed: %s\n",  
   curl_easy_strerror(res));
 }
 
 return (*httpData.get().c_str())
 
}
