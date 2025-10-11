
export abstract class BaseEntityDTO<T> {
	static fromEntity<T>(entity: T | null): T | null {
		throw new Error("fromEntity method not implemented in derived class");
	}
}