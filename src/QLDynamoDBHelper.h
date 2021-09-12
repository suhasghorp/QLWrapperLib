//
// Created by suhasghorp on 8/17/2021.
//

#ifndef QLWRAPPERLIB_INCLUDE_QLWRAPPERLIB_QLDYNAMODBHELPER_H_
#define QLWRAPPERLIB_INCLUDE_QLWRAPPERLIB_QLDYNAMODBHELPER_H_

#include <aws/core/Aws.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/utils/Array.h>
#include <aws/core/platform/Environment.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/lambda-runtime/runtime.h>
#include <json/json.hpp>

using json = nlohmann::json;

namespace dynamo {

  double getFixing(const std::string &indexName, const std::string &fixingDate);

  json getLiborRates(const std::string &valDate, const std::string &currency);

  json getConventions(const std::string &currency);

  json getRiskFreeRates(const std::string &valDate, const std::string &currency);

}

#endif//QLWRAPPERLIB_INCLUDE_QLWRAPPERLIB_QLDYNAMODBHELPER_H_
