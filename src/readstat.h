//
//  stat.h
//  Wizard
//
//  Created by Evan Miller on 3/31/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef INCLUDE_READSTAT_H
#define INCLUDE_READSTAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

typedef enum readstat_types_e {
    READSTAT_TYPE_STRING,
    READSTAT_TYPE_CHAR,
    READSTAT_TYPE_INT16,
    READSTAT_TYPE_INT32,
    READSTAT_TYPE_FLOAT,
    READSTAT_TYPE_DOUBLE,
    READSTAT_TYPE_LONG_STRING
} readstat_types_t;

typedef enum readstat_error_e {
    READSTAT_OK,
    READSTAT_ERROR_OPEN = 1,
    READSTAT_ERROR_READ,
    READSTAT_ERROR_MALLOC,
    READSTAT_ERROR_USER_ABORT,
    READSTAT_ERROR_PARSE,
    READSTAT_ERROR_UNSUPPORTED_COMPRESSION,
    READSTAT_ERROR_UNSUPPORTED_CHARSET,
    READSTAT_ERROR_ROW_COUNT_MISMATCH,
    READSTAT_ERROR_ROW_WIDTH_MISMATCH
} readstat_error_t;

const char *readstat_error_message(readstat_error_t error_code);

typedef void * readstat_value_t;

int readstat_value_is_missing(readstat_value_t value);
char readstat_char_value(readstat_value_t value);
int16_t readstat_int16_value(readstat_value_t value);
int32_t readstat_int32_value(readstat_value_t value);
float readstat_float_value(readstat_value_t value);
double readstat_double_value(readstat_value_t value);
char *readstat_string_value(readstat_value_t value);

/* Callbacks should return 0 on success and non-zero to abort */
typedef int (*readstat_info_handler)(int obs_count, int var_count, void *ctx);
typedef int (*readstat_variable_handler)(int index, const char *var_name, 
        const char *var_format, const char *var_label, const char *val_labels, 
        readstat_types_t type, void *ctx);
typedef int (*readstat_value_handler)(int obs_index, int var_index, 
        readstat_value_t value, readstat_types_t type, void *ctx);
typedef int (*readstat_value_label_handler)(const char *val_labels, 
        readstat_value_t value, readstat_types_t type, const char *label, void *ctx);
typedef void (*readstat_error_handler)(const char *error_message);

typedef struct readstat_parser_s {
    readstat_info_handler          info_handler;
    readstat_variable_handler      variable_handler;
    readstat_value_handler         value_handler;
    readstat_value_label_handler   value_label_handler;
    readstat_error_handler         error_handler;
} readstat_parser_t;

readstat_parser_t *readstat_parser_init();
void readstat_parser_free(readstat_parser_t *parser);

readstat_error_t readstat_set_info_handler(readstat_parser_t *parser, readstat_info_handler info_handler);
readstat_error_t readstat_set_variable_handler(readstat_parser_t *parser, readstat_variable_handler variable_handler);
readstat_error_t readstat_set_value_handler(readstat_parser_t *parser, readstat_value_handler value_handler);
readstat_error_t readstat_set_value_label_handler(readstat_parser_t *parser, readstat_value_label_handler value_label_handler);
readstat_error_t readstat_set_error_handler(readstat_parser_t *parser, readstat_error_handler error_handler);

readstat_error_t readstat_parse_dta(readstat_parser_t *parser, const char *filename, void *user_ctx);
readstat_error_t readstat_parse_sav(readstat_parser_t *parser, const char *filename, void *user_ctx);
readstat_error_t readstat_parse_por(readstat_parser_t *parser, const char *filename, void *user_ctx);
readstat_error_t readstat_parse_sas7bdat(readstat_parser_t *parser, const char *filename, void *user_ctx);
readstat_error_t readstat_parse_sas7bcat(readstat_parser_t *parser, const char *filename, void *user_ctx);


/* Return # bytes written or -1 for error, a la write(2) */
typedef ssize_t (*readstat_data_writer)(const void *data, size_t len, void *ctx);

typedef readstat_types_t (*readstat_variable_type_provider)(int var_index, void *ctx);
typedef const char * (*readstat_variable_name_provider)(int var_index, void *ctx);
typedef const char * (*readstat_variable_format_provider)(int var_index, void *ctx);
typedef size_t (*readstat_variable_width_provider)(int var_index, void *ctx);
typedef const void * (*readstat_value_provider)(int obs_index, int var_index, void *ctx);
typedef int (*readstat_vlabel_count_provider)(int var_index, void *ctx);
typedef void * (*readstat_vlabel_value_provider)(int label_index, int var_index, void *ctx);
typedef const char * (*readstat_vlabel_label_provider)(int label_index, int var_index, void *ctx);

/* Return non-zero at any time to abort the operation. Called before each row is emitted. */
typedef int (*readstat_cancellation_provider)(void *ctx);

typedef struct readstat_writer_s {
    readstat_data_writer                data_writer;
    readstat_variable_name_provider     variable_shortname_provider;
    readstat_variable_name_provider     variable_longname_provider;
    readstat_variable_type_provider     variable_type_provider;
    readstat_variable_format_provider   variable_format_provider;
    readstat_variable_width_provider    variable_width_provider;
    readstat_value_provider             value_provider;
    readstat_vlabel_count_provider      vlabel_count_provider;
    readstat_vlabel_value_provider      vlabel_value_provider;
    readstat_vlabel_label_provider      vlabel_label_provider;
    readstat_cancellation_provider      cancellation_provider;
    int                                 obs_count;
    int                                 var_count;
    char                                file_label[64];
} readstat_writer_t;

readstat_writer_t *readstat_writer_init();
void readstat_writer_free(readstat_writer_t *writer);

readstat_error_t readstat_set_data_writer(readstat_writer_t *writer, readstat_data_writer data_writer);
readstat_error_t readstat_set_variable_shortname_provider(readstat_writer_t *writer, readstat_variable_name_provider variable_name_provider);
readstat_error_t readstat_set_variable_longname_provider(readstat_writer_t *writer, readstat_variable_name_provider variable_name_provider);
readstat_error_t readstat_set_variable_type_provider(readstat_writer_t *writer, readstat_variable_type_provider variable_type_provider);
readstat_error_t readstat_set_variable_format_provider(readstat_writer_t *writer, readstat_variable_format_provider variable_format_provider);
readstat_error_t readstat_set_variable_width_provider(readstat_writer_t *writer, readstat_variable_width_provider variable_width_provider);
readstat_error_t readstat_set_value_provider(readstat_writer_t *writer, readstat_value_provider value_provider);
readstat_error_t readstat_set_vlabel_count_provider(readstat_writer_t *writer, readstat_vlabel_count_provider vlabel_count_provider);
readstat_error_t readstat_set_vlabel_value_provider(readstat_writer_t *writer, readstat_vlabel_value_provider vlabel_value_provider);
readstat_error_t readstat_set_vlabel_label_provider(readstat_writer_t *writer, readstat_vlabel_label_provider vlabel_label_provider);
readstat_error_t readstat_set_cancellation_provider(readstat_writer_t *writer, readstat_cancellation_provider cancellation_provider);
readstat_error_t readstat_set_row_count(readstat_writer_t *writer, int row_count);
readstat_error_t readstat_set_var_count(readstat_writer_t *writer, int var_count);
readstat_error_t readstat_set_file_label(readstat_writer_t *writer, const char *file_label);

readstat_error_t readstat_write_sav(readstat_writer_t *writer, void *user_ctx);

typedef int (*rdata_column_handler)(const char *name, readstat_types_t type, char *format, 
        void *data, long count, void *ctx);
typedef int (*rdata_table_handler)(const char *name, void *ctx);
typedef int (*rdata_text_value_handler)(const char *value, int index, void *ctx);
typedef int (*rdata_column_name_handler)(const char *value, int index, void *ctx);

typedef struct rdata_parser_s {
    rdata_table_handler         table_handler;
    rdata_column_handler        column_handler;
    rdata_column_name_handler   column_name_handler;
    rdata_text_value_handler    text_value_handler;
    rdata_text_value_handler    value_label_handler;
    readstat_error_handler      error_handler;
} rdata_parser_t;

rdata_parser_t *rdata_parser_init();
void rdata_parser_free(rdata_parser_t *parser);

readstat_error_t rdata_set_table_handler(rdata_parser_t *parser, rdata_table_handler table_handler);
readstat_error_t rdata_set_column_handler(rdata_parser_t *parser, rdata_column_handler column_handler);
readstat_error_t rdata_set_column_name_handler(rdata_parser_t *parser, rdata_column_name_handler column_name_handler);
readstat_error_t rdata_set_text_value_handler(rdata_parser_t *parser, rdata_text_value_handler text_value_handler);
readstat_error_t rdata_set_value_label_handler(rdata_parser_t *parser, rdata_text_value_handler value_label_handler);
readstat_error_t rdata_set_error_handler(rdata_parser_t *parser, readstat_error_handler error_handler);
/* rdata_parse works on RData and RDS. The table handler will be called once
 * per data frame in RData files, and zero times on RDS files. */
readstat_error_t rdata_parse(rdata_parser_t *parser, const char *filename, void *user_ctx);

#ifdef __cplusplus
}
#endif

#endif
