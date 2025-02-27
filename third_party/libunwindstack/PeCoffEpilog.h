/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LIBUNWINDSTACK_PE_COFF_EPILOG_H
#define _LIBUNWINDSTACK_PE_COFF_EPILOG_H

#include <vector>

#include <capstone/capstone.h>

#include <unwindstack/Error.h>
#include <unwindstack/Memory.h>
#include <unwindstack/Regs.h>

namespace unwindstack {

// Helper class for epilog detection and handling used for X86_64 unwinding for PE/COFF modules.
class PeCoffEpilog {
 public:
  PeCoffEpilog(Memory* file_memory, uint64_t text_section_vmaddr, uint64_t text_section_offset)
      : file_memory_(file_memory),
        text_section_vmaddr_(text_section_vmaddr),
        text_section_offset_(text_section_offset) {}
  ~PeCoffEpilog();

  // Needs to be called before one can use DetectAndHandleEpilog.
  bool Init();

  // Detects if the instructions from 'current_offset_from_start_of_function' onwards represent
  // a function epilog. Returns true if an epilog was detected. The registers are updated to reflect
  // the actions from executing the epilog (which effectively unwinds the current callframe).
  // Returns false if no epilog was found *or* if an error occured. In the latter case, the error
  // can be retrieved using GetLastError() and registers 'regs' are not updated.
  bool DetectAndHandleEpilog(uint64_t function_start_address, uint64_t function_end_address,
                             uint64_t current_offset_from_start_of_function, Memory* process_memory,
                             Regs* regs);

  ErrorData GetLastError() const { return last_error_; }

 private:
  bool DetectAndHandleEpilog(const std::vector<uint8_t>& machine_code, Memory* process_memory,
                             Regs* regs);

  // The validation methods below check if the instructions satisfy the requirements imposed by the
  // epilog specification, as outlined on
  // https://docs.microsoft.com/en-us/cpp/build/prolog-and-epilog?view=msvc-170
  // The corresponding handling methods must only be called after the validation was successful.
  bool ValidateLeaInstruction();
  void HandleLeaInstruction(RegsImpl<uint64_t>* registers);
  bool ValidateAddInstruction();
  void HandleAddInstruction(RegsImpl<uint64_t>* registers);
  bool HandlePopInstruction(Memory* process_memory, RegsImpl<uint64_t>* registers);
  bool HandleReturnInstruction(Memory* process_memory, RegsImpl<uint64_t>* registers);
  bool ValidateJumpInstruction();
  bool HandleJumpInstruction(Memory* process_memory, RegsImpl<uint64_t>* registers);

  Memory* file_memory_;
  uint64_t text_section_vmaddr_ = 0;
  uint64_t text_section_offset_ = 0;

  ErrorData last_error_{ERROR_NONE, 0};

  bool capstone_initialized_ = false;
  csh capstone_handle_ = 0;
  cs_insn* capstone_instruction_ = nullptr;
};

}  // namespace unwindstack

#endif  // _LIBUNWINDSTACK_PE_COFF_EPILOG_H