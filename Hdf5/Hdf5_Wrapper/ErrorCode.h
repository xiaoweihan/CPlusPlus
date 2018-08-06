#ifndef ERROR_CODE_H
#define ERROR_CODE_H

enum ERROR_CODE
{
	ERROR_NO_ERROR = 0,
	ERROR_PARAM_INVALID,
	ERROR_FILE_NOT_EXIST,
	ERROR_WRITE_HDF5_FAILED,
	ERROR_JUDGE_FILE_EXCEPTION,
	ERROR_FILE_FORMAT_INVALID,
	ERROR_READ_HDF5_FAILED,
	ERROR_SIGNATURE_EXPIRED,
	ERROR_ALLOCATE_FAILED,
	ERROR_PARSE_COMPOUND_TYPE_FAILED,
	ERROR_DATA_TYPE_FAILED,
	ERROR_DATA_HEADER_INVALID,
	ERROR_CONVERT_NODE_TYPE_FAILED,
	ERROR_CONVERT_INT_TYPE_HEADER_FAILED,
	ERROR_CONVERT_COMPOUND_TYPE_HEADER_FAILED,
	ERROR_NOT_SUPPORT_DATA_TYPE,
	ERROR_RANK_NOT_SUPPORT,
	ERROR_INDEX_OUT_OF_RANGE

};
#endif