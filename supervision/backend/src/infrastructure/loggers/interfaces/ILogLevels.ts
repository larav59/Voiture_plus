import chalk from "chalk";

export interface ILogLevels<T> {
	info: T;
	debug: T;
	success: T;
	warning: T;
	error: T;
}

export type LogLevel = keyof ILogLevels<any>; // 'info' | 'success' | 'warning' | 'error'
export type IColoredLogLevels = ILogLevels<chalk.ChalkFunction>;
