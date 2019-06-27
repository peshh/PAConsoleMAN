#pragma once

int orderNumbersByDescending(const void* a, const void* b)
{
	return (*(int*)b - *(int*)a);
}