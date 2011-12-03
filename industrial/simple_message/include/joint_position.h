﻿/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Southwest Research Institute
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 	* Redistributions of source code must retain the above copyright
 * 	notice, this list of conditions and the following disclaimer.
 * 	* Redistributions in binary form must reproduce the above copyright
 * 	notice, this list of conditions and the following disclaimer in the
 * 	documentation and/or other materials provided with the distribution.
 * 	* Neither the name of the Southwest Research Institute, nor the names
 *	of its contributors may be used to endorse or promote products derived
 *	from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef JOINT_POSITION_H
#define JOINT_POSITION_H

#include "simple_message.h"
#include "simple_serialize.h"
#include "shared_types.h"
//#include "joint_message.h"
/*
namespace industrial
{
namespace joint_message
{
// Class declaration required for function prototypes below
class JointMessage;
}
}
*/

namespace industrial
{
namespace joint_position
{

/**
 * \brief Class encapsulated joint position data.  For simplicity and cross platform
 * compliance, this is implemented as a fixed size array.
 */
//* JointMessage
/**
 *
 * THIS CLASS IS NOT THREAD-SAFE
 *
 */

class JointPosition : public industrial::simple_serialize::SimpleSerialize
{
public:
  /**
   * \brief Default constructor
   *
   * This method creates empty data.
   *
   */
  JointPosition(void);
  /**
   * \brief Destructor
   *
   */
  ~JointPosition(void);

  /**
   * \brief Initializes a empty joint data
   *
   */
  void init();

  /**
   * \brief Initializes joint position from joint message
   *
   */
  //void init(industrial::joint_message::JointMessage& joint_msg);

  /**
   * \brief Sets a joint value within the buffer
   *
   * \param joint index
   * \param joint value
   *
   * \return true if value set, otherwise false (index greater than max)
   */
  bool setJoint(industrial::shared_types::shared_int index, industrial::shared_types::shared_real value);

  /**
   * \brief Gets a joint value within the buffer
   *
   * \param joint index
   * \param joint value (passed by reference)
   *
   * \return true if value valid, otherwise false (index greater than max)
   */
  bool getJoint(industrial::shared_types::shared_int index, industrial::shared_types::shared_real & value);
  
  /**
   * \brief Gets a joint value within the buffer (Only use this form if you are
   * sure the index is within bounds).
   *
   * \param joint index
   *
   * \return joint value (returns 0.0 if index is out of bounds)
   */
  industrial::shared_types::shared_real getJoint(industrial::shared_types::shared_int index);
  
  
  /**
   * \brief Copies the passed in value
   *
   * \param src (value to copy)
   */
  void copyFrom(JointPosition &src);

  /**
   * \brief returns the maximum number of joints the message holds
   *
   * \return max number of joints
   */
  int getMaxNumJoints() const
  {
    return MAX_NUM_JOINTS;
  }

  /**
     * \brief == operator implementation
     *
     * \return true if equal
     */
  bool operator==(JointPosition &rhs);

  // Overrides - SimpleSerialize
  bool load(industrial::byte_array::ByteArray *buffer);
  bool unload(industrial::byte_array::ByteArray *buffer);
  unsigned int byteLength()
  {
    return MAX_NUM_JOINTS * sizeof(industrial::shared_types::shared_real);
  }

private:

  /**
   * \brief maximum number of joints positions that can be held in the message.
   */
  static const industrial::shared_types::shared_int MAX_NUM_JOINTS = 10;
  /**
   * \brief internal data buffer
   */
  industrial::shared_types::shared_real joints_[MAX_NUM_JOINTS];

};

}
}

#endif /* JOINT_POSITION_H */