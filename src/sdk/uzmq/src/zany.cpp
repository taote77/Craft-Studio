#include "uzmq/zany.h"

#include <boost/algorithm/hex.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <iomanip>
#include <sstream>

namespace uzmq
{

ZAny::ZAny()
{

}

ZAny::ZAny(const std::string &str):boost::json::value(str)
{

}

ZAny::~ZAny()
{

}

ZAny& ZAny::operator=(const boost::json::value& value)
{
    static_cast<boost::json::value&>(*this) = boost::json::value(value);
    return *this;
}


std::string ZAny::encodeMat(const cv::Mat &mat)
{
    if (mat.empty())
    {
        return "";
    }
    else
    {
        std::vector<uchar> buf;
        cv::imencode(".png", mat, buf);
        std::string str(buf.begin(), buf.end());
        return ZAny::base64Encode(str);
    }
}

cv::Mat ZAny::decodeMat(const std::string &content)
{
    if(content.empty())
    {
        return cv::Mat();
    }
    else
    {
        std::string str = ZAny::base64Decode(content);
        std::vector<uchar> vec(str.begin(), str.end());
        return cv::imdecode(vec, cv::IMREAD_UNCHANGED);
    }
}


std::string ZAny::serialize() const
{
    return boost::json::serialize(*this);
}

void ZAny::deserialize(const std::string& str)
{
    boost::json::parse_options opt;
    opt.allow_invalid_utf8 = true;
    *this = boost::json::parse(str, {}, opt);
}

std::string ZAny::base64Encode(const std::string &in)
{
    typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8>> base64_enc_iter;
    std::stringstream os;
    std::copy(base64_enc_iter(in.begin()), base64_enc_iter(in.end()), std::ostream_iterator<char>(os));
    size_t num_padding_chars = (3 - in.length() % 3) % 3;
    for (size_t i = 0; i < num_padding_chars; ++i) {
        os.put('=');
    }
    return os.str();
}

std::string ZAny::base64Decode(const std::string &in)
{
    typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> base64_dec_iter;
    std::stringstream os;
    size_t in_len = in.length();
    size_t num_padding_chars = 0;
    if (in_len > 0 && in[in_len - 1] == '=') {
        num_padding_chars++;
    }
    if (in_len > 1 && in[in_len - 2] == '=') {
        num_padding_chars++;
    }
    std::copy(base64_dec_iter(in.begin()), base64_dec_iter(in.end() - num_padding_chars), std::ostream_iterator<char>(os));
    return os.str();
}

std::ostream& operator<<(std::ostream& os, const ZAny& data)
{
    os << data.serialize();
    return os;
}

}    // namespace uzmq
