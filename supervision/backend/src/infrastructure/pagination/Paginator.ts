import { IPaginatedData } from './interfaces/IPaginatedData';

export class Paginator<T> {
	private items: T[];
	private pageSize: number;
	private total: number;
	private page: number;

	constructor(items: T[], pageSize: number, page: number) {
		this.items = items;
		this.pageSize = pageSize;
		this.total = items.length;
		this.page = page;
	}

	public getPaginatedData() {
		return {
			totalItems: this.total,
			totalPages: Math.ceil(this.total / this.pageSize),
			currentPage: this.page,
			pageSize: this.pageSize,
			items: this.items.slice((this.page - 1) * this.pageSize, this.page * this.pageSize)
		} as IPaginatedData<T>;
	}
}