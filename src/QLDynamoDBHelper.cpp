//
// Created by suhasghorp on 8/22/2021.
//

#include "QLDynamoDBHelper.h"


  std::function<std::shared_ptr<Aws::Utils::Logging::LogSystemInterface>()> GetConsoleLoggerFactory() {
    return [] {
      return Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(
          "console_logger", Aws::Utils::Logging::LogLevel::Trace);
    };
  }

  double dynamo::getFixing(const std::string &indexName, const std::string &fixingDate) {
    using namespace Aws;
    using namespace Aws::DynamoDB;
    using namespace Aws::DynamoDB::Model;

    double fixing = 0.0;
    SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    options.loggingOptions.logger_create_fn = GetConsoleLoggerFactory();
    InitAPI(options);
    {
      Aws::Client::ClientConfiguration config;
      auto credentialsProvider = Aws::MakeShared<Aws::Auth::EnvironmentAWSCredentialsProvider>("dynamodb-helper");
      Aws::DynamoDB::DynamoDBClient client(credentialsProvider, config);

      QueryRequest query;

      auto const &table_name = "libor-fixings";
      query.SetTableName(table_name);
      query.SetKeyConditionExpression("#H = :h");
      query.AddExpressionAttributeNames("#H", "index-name");
      query.AddExpressionAttributeValues(":h", AttributeValue(indexName));
      auto outcome = client.Query(query);
      if (outcome.IsSuccess()) {
        const Aws::Vector<Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>> &items = outcome.GetResult().GetItems();
        if (!items.empty()) {
          for (const auto &i : items[0]) {
            if (i.first == "fixings") {
              json j = json::parse(i.second.GetS());
              json filtered;
              std::copy_if(j["fixings"].begin(), j["fixings"].end(),
                           std::back_inserter(filtered), [&fixingDate](const json &item) {
                             return item.contains("val_date") && item["val_date"] == fixingDate;
                           });
              std::cout << "filtered:" << filtered.at(0).at("rate").dump() << std::endl;
              json val;
              for (auto &el : filtered.items()) {
                val = el.value();
                fixing = val.at("rate").get<double>();
              }
            }
          }
        } else {
          std::cout << "No item found in table: " << table_name << std::endl;
        }
      }
    }
    ShutdownAPI(options);// never put debug stop here
    return fixing / 100.0;
  }

  json dynamo::getRiskFreeRates(const std::string &valDate, const std::string &currency) {
    using namespace Aws;
    using namespace Aws::DynamoDB;
    using namespace Aws::DynamoDB::Model;

    json jrates;
    SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    options.loggingOptions.logger_create_fn = GetConsoleLoggerFactory();
    InitAPI(options);
    {
      Aws::Client::ClientConfiguration config;
      config.region = Aws::Environment::GetEnv("AWS_REGION");
      config.caFile = "/etc/pki/tls/certs/ca-bundle.crt";
      config.disableExpectHeader = true;
      auto credentialsProvider = Aws::MakeShared<Aws::Auth::EnvironmentAWSCredentialsProvider>("dynamodb-helper");
      Aws::DynamoDB::DynamoDBClient client(credentialsProvider, config);

      QueryRequest query;
      auto const &table_name = currency == "USD" ? "usd-sofr" : "eur-estr";
      query.SetTableName(table_name);
      query.SetKeyConditionExpression("#H = :h");
      query.AddExpressionAttributeNames("#H", "val-date");
      query.AddExpressionAttributeValues(":h", AttributeValue(valDate));
      auto outcome = client.Query(query);
      if (outcome.IsSuccess()) {
        //auto const &maps = outcome.GetResult().GetItems();// returns vector of map<column, attibute value>
        const Aws::Vector<Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>> &items = outcome.GetResult().GetItems();
        if (!items.empty()) {
          for (const auto &i : items[0]) {
            if (i.first == "rates")
              jrates = json::parse(i.second.GetS());
          }
        } else {
          std::cout << "No item found in table: " << table_name << std::endl;
        }
      }
    }
    ShutdownAPI(options);
    std::cout << "getriskfreerates : " << jrates.dump() << std::endl;
    return jrates;
  }

  json dynamo::getLiborRates(const std::string &valDate, const std::string &currency) {

    using namespace Aws;
    using namespace Aws::DynamoDB;
    using namespace Aws::DynamoDB::Model;

    json jrates;
    SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    options.loggingOptions.logger_create_fn = GetConsoleLoggerFactory();
    InitAPI(options);
    {
      Aws::Client::ClientConfiguration config;
      config.region = Aws::Environment::GetEnv("AWS_REGION");
      config.caFile = "/etc/pki/tls/certs/ca-bundle.crt";
      config.disableExpectHeader = true;
      auto credentialsProvider = Aws::MakeShared<Aws::Auth::EnvironmentAWSCredentialsProvider>("dynamodb-helper");
      Aws::DynamoDB::DynamoDBClient client(credentialsProvider, config);

      QueryRequest query;

      auto const &table_name = "usd-libor-3m";
      query.SetTableName(table_name);
      query.SetKeyConditionExpression("#H = :h");
      query.AddExpressionAttributeNames("#H", "val-date");
      query.AddExpressionAttributeValues(":h", AttributeValue(valDate));
      auto outcome = client.Query(query);
      if (outcome.IsSuccess()) {
        //auto const &maps = outcome.GetResult().GetItems();// returns vector of map<column, attibute value>
        const Aws::Vector<Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>> &items = outcome.GetResult().GetItems();
        if (!items.empty()) {
          for (const auto &i : items[0]) {
            if (i.first == "rates")
              jrates = json::parse(i.second.GetS());
          }
        } else {
          std::cout << "No item found in table: " << table_name << std::endl;
        }
      }
    }
    ShutdownAPI(options);
    return jrates;
  }

  json dynamo::getConventions(const std::string &currency) {
    using namespace Aws;
    using namespace Aws::DynamoDB;
    using namespace Aws::DynamoDB::Model;

    json jconventions;
    SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    options.loggingOptions.logger_create_fn = GetConsoleLoggerFactory();
    InitAPI(options);
    {
      Aws::Client::ClientConfiguration config;
      config.region = Aws::Environment::GetEnv("AWS_REGION");
      config.caFile = "/etc/pki/tls/certs/ca-bundle.crt";
      config.disableExpectHeader = true;
      auto credentialsProvider = Aws::MakeShared<Aws::Auth::EnvironmentAWSCredentialsProvider>("dynamodb-helper");
      Aws::DynamoDB::DynamoDBClient client(credentialsProvider, config);

      QueryRequest query;

      auto const &table_name = "conventions";
      query.SetTableName(table_name);
      query.SetKeyConditionExpression("#H = :h");
      query.AddExpressionAttributeNames("#H", "currency");
      query.AddExpressionAttributeValues(":h", AttributeValue(currency));
      auto outcome = client.Query(query);
      if (outcome.IsSuccess()) {
        const Aws::Vector<Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>> &items = outcome.GetResult().GetItems();
        if (!items.empty()) {
          for (const auto &i : items[0]) {
            if (i.first == "conventions") {
              jconventions = json::parse(i.second.GetS());
            }
          }
        } else {
          std::cout << "No item found in table: " << table_name << std::endl;
        }
      }
    }
    ShutdownAPI(options);
    return jconventions;
  }

