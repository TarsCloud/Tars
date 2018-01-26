<!--
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
-->

<div class="frame-sidebar">
    <div class="ml10 side">
        <div class="list-group">
            <a href="/server_deploy.html" target="main_iframe" class="list-group-item active">服务部署</a>
            <a href="/server_expand.html" target="main_iframe" class="list-group-item">扩容</a>
            <a href="/template_manage.html" target="main_iframe" class="list-group-item">模板管理</a>
        </div>
    </div>
</div>

<script>
    $('.list-group').on('click','a',function () {
        $(this).siblings().removeClass('active');
        $(this).addClass('active');
    });
</script>
