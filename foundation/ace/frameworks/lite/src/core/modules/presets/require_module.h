/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_ACELITE_REQUIRE_MODULE_H
#define OHOS_ACELITE_REQUIRE_MODULE_H

#include "non_copyable.h"
#include "presets/preset_module.h"

namespace OHOS {
namespace ACELite {
class RequireModule final : public PresetModule {
public:
    ACE_DISALLOW_COPY_AND_MOVE(RequireModule);
    /**
     * @fn RequireModule::RequireModule()
     *
     * @brief Constructor.
     */
    RequireModule() : PresetModule(nullptr) {}

    /**
     * @fn RequireModule::~RequireModule()
     *
     * @brief Constructor.
     */
    ~RequireModule() = default;

    void Init() override;

    static void Load()
    {
        RequireModule requireModule;
        requireModule.Init();
    }

private:
    static jerry_value_t ImportModule(const jerry_value_t func,
                                      const jerry_value_t obj,
                                      const jerry_value_t *args,
                                      const jerry_length_t argsNum);
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_REQUIRE_MODULE_H
