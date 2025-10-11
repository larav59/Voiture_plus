export interface IPaginatedData<T> {
	totalItems: number;
	totalPages: number;
	currentPage: number;
	pageSize: number;
	items: T[];
}
