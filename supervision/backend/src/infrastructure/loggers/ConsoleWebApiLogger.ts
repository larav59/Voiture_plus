import chalk from "chalk";
import { Logger } from "./Logger";
import { IColoredLogLevels, LogLevel } from "./interfaces/ILogLevels";
import moment from "moment";

const levels: IColoredLogLevels = {
	info: chalk.blue,
	success: chalk.green,
	warning: chalk.yellow,
	error: chalk.red,
	debug: chalk.magenta,
};

class ConsoleWebApiLogger extends Logger {
	private static instance: ConsoleWebApiLogger;

	public static getInstance(): ConsoleWebApiLogger {
		if (!ConsoleWebApiLogger.instance) {
			ConsoleWebApiLogger.instance = new ConsoleWebApiLogger();
		}
		return ConsoleWebApiLogger.instance;
	}

	public log(level: LogLevel, ...args: any[]): void {
		const timestamp = moment().format("YYYY-MM-DD HH:mm:ss.SSS");
		const color = levels[level] || chalk.white;

		const prefix = color(`[${timestamp}][${level.toUpperCase()}]`);

		console.log(prefix, ...args);
	}
}

export default ConsoleWebApiLogger.getInstance();
