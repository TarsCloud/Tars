/*
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
 */

package com.qq.common;

import org.apache.commons.lang3.StringUtils;
import org.dozer.Mapper;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.MissingServletRequestParameterException;
import org.springframework.web.bind.ServletRequestBindingException;
import org.springframework.web.bind.UnsatisfiedServletRequestParameterException;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.servlet.ModelAndView;

import javax.servlet.http.HttpServletRequest;
import javax.validation.ConstraintViolation;
import javax.validation.Validation;
import javax.validation.Validator;
import javax.validation.ValidatorFactory;
import java.util.Set;

public class WrappedController {

    public static final int EC_SUCCESS = 200;
    public static final int EC_PARAMETER_WRONG = 400;
    public static final int EC_INTERNAL_DEFAULT = 500;
    private final Logger log = LoggerFactory.getLogger(WrappedController.class);

    @Autowired
    protected Mapper mapper;

    @ExceptionHandler(value = {
            MethodArgumentNotValidException.class,
            MissingServletRequestParameterException.class,
            ServletRequestBindingException.class,
            UnsatisfiedServletRequestParameterException.class,
            ServletRequestParameterException.class
    })
    @ResponseBody
    public Object handleRequestFormatException(HttpServletRequest request, Exception e) {
        boolean isAjax = isAjaxRequest(request);
        log.info("exception intercepted, uri={}, isajax={}", request.getRequestURI(), isAjax, e);

        String msg = e.getMessage();
        if (e instanceof MethodArgumentNotValidException) {
            msg = BindingResultMessage.get(((MethodArgumentNotValidException) e).getBindingResult());
        }

        ExceptionResponse response = new ExceptionResponse(EC_SUCCESS, EC_PARAMETER_WRONG, msg);
        response.setAttached(new ExceptionResponseAttached().loadFromRequest(request));

        if (isAjax) {
            return response;
        } else {
            ModelAndView mv = new ModelAndView();
            mv.setViewName("exception");
            mv.addObject("response", response);
            return mv;
        }
    }

    @ExceptionHandler(Exception.class)
    @ResponseBody
    public Object handleException(HttpServletRequest request, Exception e) {
        boolean isAjax = isAjaxRequest(request);
        log.info("exception intercepted, uri={}, isajax={}", request.getRequestURI(), isAjax, e);

        ExceptionResponse response;
        if (e instanceof CodeEnabled) {
            CodeEnabled ce = (CodeEnabled) e;
            response = new ExceptionResponse(EC_INTERNAL_DEFAULT, ce.getFullCode(), ce.getMessageForUser());
        } else {
            response = new ExceptionResponse(EC_INTERNAL_DEFAULT, StringUtils.EMPTY, e.getMessage());
        }
        response.setAttached(new ExceptionResponseAttached().loadFromRequest(request));

        if (isAjax) {
            return response;
        } else {
            ModelAndView mv = new ModelAndView();
            mv.setViewName("exception");
            mv.addObject("response", response);
            return mv;
        }
    }

    private boolean isAjaxRequest(HttpServletRequest request) {
        String requestedWith = request.getHeader("X-Requested-With");
        return requestedWith != null && "XMLHttpRequest".equals(requestedWith);
    }

    protected void validate(Object bean, Class<?>... groups) throws ServletRequestBindingException {
        ValidatorFactory validatorFactory = Validation.buildDefaultValidatorFactory();
        Validator validator = validatorFactory.getValidator();
        Set<ConstraintViolation<Object>> constraintViolations = validator.validate(bean, groups);
        if (!constraintViolations.isEmpty()) {
            StringBuilder sb = new StringBuilder();
            for (ConstraintViolation<Object> constraint : constraintViolations) {
                sb.append(String.format("%s %s\n", constraint.getPropertyPath(), constraint.getMessage()));
            }
            throw new ServletRequestBindingException(sb.toString());
        }
    }
}
