#ifndef ZANY_H
#define ZANY_H

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/boost_unordered_map.hpp>
#include <boost/serialization/boost_unordered_set.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/queue.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <future>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <type_traits>

namespace uzmq {
class ZAny : public boost::json::value
{
public:
    using boost::json::value::value;

    ZAny();

    ZAny(const std::string &str);

    virtual ~ZAny();

    template <typename T>
    ZAny &operator=(const T &value)
    {
        static_cast<boost::json::value &>(*this) = boost::json::value(value);
        return *this;
    }

    ZAny &operator=(const boost::json::value &value);

    template <typename T>
    static ZAny valueFrom(const T &value)
    {
        ZAny reply;
        try
        {
            std::ostringstream            oss;
            boost::archive::text_oarchive oa(oss);
            oa << value;
            reply = oss.str();
            return reply;
        }
        catch (std::exception &e)
        {
            throw std::runtime_error(std::string("Failed to value from this type.Error:") + e.what());
        }
    }

    template <typename T>
    T valueTo()
    {
        try
        {
            std::string                   str = (*this).as_string().c_str();
            T                             data;
            std::istringstream            iss(str);
            boost::archive::text_iarchive ia(iss);
            ia >> data;
            return data;
        }
        catch (std::exception &e)
        {
            throw std::runtime_error("Failed to value to this type.Json:" + serialize() + ",Error:" + e.what());
        }
    }

    std::string serialize() const;

    void deserialize(const std::string &str);

    static std::string base64Encode(const std::string &in);

    static std::string base64Decode(const std::string &in);

    static std::string encodeMat(const cv::Mat &mat);

    static cv::Mat decodeMat(const std::string &content);
};

std::ostream &operator<<(std::ostream &os, const ZAny &msg);
} // namespace uzmq

namespace boost {
namespace serialization {
template <class Archive>
static void save(Archive &ar, const cv::Mat &mat, const unsigned int version)
{
    std::string strMat = uzmq::ZAny::encodeMat(mat);
    ar &        strMat;
}

template <class Archive>
static void load(Archive &ar, cv::Mat &mat, const unsigned int version)
{
    std::string strMat;
    ar &        strMat;
    mat = uzmq::ZAny::decodeMat(strMat);
}

template <class Archive>
void serialize(Archive &ar, cv::Mat &mat, const unsigned int version)
{
    split_free(ar, mat, version);
}

template <class Archive>
static void save(Archive &ar, const uzmq::ZAny &data, const unsigned int version)
{
    std::string str = data.serialize();
    ar &        str;
}

template <class Archive>
static void load(Archive &ar, uzmq::ZAny &data, const unsigned int version)
{
    std::string str;
    ar &        str;
    data.deserialize(str);
}

template <class Archive>
void serialize(Archive &ar, uzmq::ZAny &data, const unsigned int version)
{
    split_free(ar, data, version);
}
template <class Archive>
static void save(Archive &ar, const boost::json::object &obj, const unsigned int version)
{
    boost::json::value jsonValue = obj;
    auto               str       = boost::json::serialize(jsonValue);
    ar &               str;
}

template <class Archive>
static void load(Archive &ar, boost::json::object &obj, const unsigned int version)
{
    std::string        jsonStr;
    ar &               jsonStr;
    boost::json::value jsonValue = boost::json::parse(jsonStr);
    obj                          = boost::json::value_to<boost::json::object>(jsonValue);
}

template <class Archive>
void serialize(Archive &ar, boost::json::object &obj, const unsigned int version)
{
    split_free(ar, obj, version);
}
}
} // namespace boost::serialization

#endif // ZANY_H
