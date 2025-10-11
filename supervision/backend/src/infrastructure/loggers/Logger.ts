// src/infrastructure/logger/Logger.ts
import { config } from "../../config";
import { LogLevel } from "./interfaces/ILogLevels";

export abstract class Logger {
	public abstract log(level: LogLevel, ...args: any[]): void;

	public info(...args: any[]): void {
		this.log("info", ...args);
	}

	public success(...args: any[]): void {
		this.log("success", ...args);
	}

	public warning(...args: any[]): void {
		this.log("warning", ...args);
	}

	public error(...args: any[]): void {
		this.log("error", ...args);
	}

	public debug(...args: any[]): void {
		if (config.env !== "production") {
			this.log("debug", ...args);
		}
	}
}
