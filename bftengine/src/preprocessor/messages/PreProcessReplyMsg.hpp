// Concord
//
// Copyright (c) 2019-2021 VMware, Inc. All Rights Reserved.
//
// This product is licensed to you under the Apache 2.0 license (the "License"). You may not use this product except in
// compliance with the Apache 2.0 License.
//
// This product may include a number of subcomponents with separate copyright notices and license terms. Your use of
// these subcomponents is subject to the terms and conditions of the sub-component's license, as noted in the LICENSE
// file.

#pragma once

#include "messages/MessageBase.hpp"
#include "../PreProcessorRecorder.hpp"
#include "sha_hash.hpp"
#include <memory>

namespace preprocessor {

typedef enum { STATUS_GOOD, STATUS_REJECT } ReplyStatus;

class PreProcessReplyMsg : public MessageBase {
 public:
  PreProcessReplyMsg(NodeIdType senderId,
                     uint16_t clientId,
                     uint16_t reqOffsetInBatch,
                     uint64_t reqSeqNum,
                     uint64_t reqRetryId,
                     const char* preProcessResultBuf,
                     uint32_t preProcessResultBufLen,
                     const std::string& cid,
                     ReplyStatus status);

  PreProcessReplyMsg(NodeIdType senderId,
                     uint16_t clientId,
                     uint16_t reqOffsetInBatch,
                     uint64_t reqSeqNum,
                     uint64_t reqRetryId,
                     const uint8_t* resultsHash,
                     const char* signature,
                     const std::string& cid,
                     ReplyStatus status);

  BFTENGINE_GEN_CONSTRUCT_FROM_BASE_MESSAGE(PreProcessReplyMsg)

  void validate(const bftEngine::impl::ReplicasInfo&) const override;
  const uint16_t clientId() const { return msgBody()->clientId; }
  const uint16_t reqOffsetInBatch() const { return msgBody()->reqOffsetInBatch; }
  const SeqNum reqSeqNum() const { return msgBody()->reqSeqNum; }
  const uint64_t reqRetryId() const { return msgBody()->reqRetryId; }
  const uint32_t replyLength() const { return msgBody()->replyLength; }
  const uint8_t* resultsHash() const { return msgBody()->resultsHash; }
  const uint8_t status() const { return msgBody()->status; }
  std::string getCid() const;
  static void setPreProcessorHistograms(preprocessor::PreProcessorRecorder* histograms) {
    preProcessorHistograms_ = histograms;
  }

 public:
#pragma pack(push, 1)
  struct Header {
    MessageBase::Header header;
    SeqNum reqSeqNum;
    NodeIdType senderId;
    uint16_t clientId;
    uint16_t reqOffsetInBatch;
    uint8_t status;
    uint8_t resultsHash[concord::util::SHA3_256::SIZE_IN_BYTES];
    uint32_t replyLength;
    uint32_t cidLength;
    uint64_t reqRetryId;
  };
// The pre-executed results' hash signature resides in the message body
#pragma pack(pop)

 protected:
  template <typename MessageT>
  friend size_t bftEngine::impl::sizeOfHeader();

 private:
  static logging::Logger& logger() {
    static logging::Logger logger_ = logging::getLogger("concord.preprocessor");
    return logger_;
  }
  void setParams(NodeIdType senderId,
                 uint16_t clientId,
                 uint16_t reqOffsetInBatch,
                 ReqId reqSeqNum,
                 uint64_t reqRetryId,
                 ReplyStatus status);
  void setupMsgBody(const char* preProcessResultBuf,
                    uint32_t preProcessResultBufLen,
                    const std::string& cid,
                    ReplyStatus status);
  void setupMsgBody(const uint8_t* resultsHash, const char* signature, const std::string& cid);
  void setLeftMsgParams(const std::string& cid, uint16_t sigSize);
  Header* msgBody() const { return ((Header*)msgBody_); }

 private:
  static uint16_t maxReplyMsgSize_;
  inline static preprocessor::PreProcessorRecorder* preProcessorHistograms_ = nullptr;
};

typedef std::shared_ptr<PreProcessReplyMsg> PreProcessReplyMsgSharedPtr;

}  // namespace preprocessor
