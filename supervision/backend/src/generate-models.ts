/**
 * utility script to update models in the database
 */
import dotenv from 'dotenv';
dotenv.config({
    quiet: true,
})

import exec from 'child_process';
import path from 'path';
import { mysqlConfig } from './config';

function neutralize(str: string): string {
    // neutralize parameters for linux shell
    // surround with quotes
    return `'${str}'`;
}


const parameters: Map<string, string> = new Map();
parameters.set('-h', mysqlConfig.host);
parameters.set('-d', mysqlConfig.database);
parameters.set('-u', mysqlConfig.user);
parameters.set('-x', mysqlConfig.password);
parameters.set('-p', mysqlConfig.port.toString());
parameters.set('-e', 'mysql');
parameters.set('-o', path.resolve(__dirname, 'data/entities'));
parameters.set('--noConfig', 'true');
parameters.set('--property-visibility', 'public');
parameters.set('--strictMode', '!');
parameters.set('--generateConstructor', 'true');

let args = '';

parameters.forEach((value, key) => {
    // neutralize parameters only if they are not boolean (true/false) or number (port)
    let val = value;
    if (value !== 'true' && value !== 'false' && isNaN(parseInt(value))) {
        val = neutralize(value);
    }
    args += `${key} ${val} `;
});

const command = `npx typeorm-model-generator ${args}`;

exec.exec(command, (err, stdout, stderr) => {
    if (err) {
        console.error(err);
        return;
    }
    console.log(stdout);
});

/*
 * Options disponibles pour typeorm-model-generator :
 * 
 * -h, --host: IP address/Hostname for database server (string) [default: "127.0.0.1"]
 * -d, --database: Database name (or path for sqlite). You can pass multiple values separated by comma. (string) [required]
 * -u, --user: Username for database server (string) [default: ""]
 * -x, --pass: Password for database server (string) [default: ""]
 * -p, --port: Port number for database server (number) [default: 0]
 * -e, --engine: Database engine (required) [choices: "mssql", "postgres", "mysql", "mariadb", "oracle", "sqlite"]
 * -o, --output: Where to place generated models (string) [default: "/home/lukas/nerd-bot/output"]
 * -s, --schema: Schema name to create model from. Only for mssql and postgres. (string) [default: ""]
 * -i, --instance: Named instance to create model from. Only for mssql. (string)
 * --ssl: Enable SSL connection (boolean) [default: false]
 * --noConfig: Doesn't create tsconfig.json and ormconfig.json (boolean) [default: false]
 * --cf, --case-file: Convert file names to specified case [choices: "pascal", "param", "camel", "none"] [default: "pascal"]
 * --ce, --case-entity: Convert class names to specified case [choices: "pascal", "camel", "none"] [default: "pascal"]
 * --cp, --case-property: Convert property names to specified case [choices: "pascal", "camel", "snake", "none"] [default: "camel"]
 * --eol: Force EOL to be LF or CRLF [choices: "LF", "CRLF"] [default: "LF"]
 * --pv, --property-visibility: Defines which visibility should have the generated property [choices: "public", "protected", "private", "none"] [default: "none"]
 * --lazy: Generate lazy relations (boolean) [default: false]
 * -a, --active-record: Use ActiveRecord syntax for generated models (boolean) [default: false]
 * --namingStrategy: Use custom naming strategy (string) [default: ""]
 * --relationIds: Generate RelationId fields (boolean) [default: false]
 * --skipSchema: Omits schema identifier in generated entities (boolean) [default: false]
 * --generateConstructor: Generate constructor allowing partial initialization (boolean) [default: false]
 * --disablePluralization: Disable pluralization of OneToMany, ManyToMany relation names (boolean) [default: false]
 * --skipTables: Skip schema generation for specific tables. You can pass multiple values separated by comma (string) [default: ""]
 * --tables: Generate specific tables. You can pass multiple values separated by comma (string) [default: ""]
 * --strictMode: Mark fields as optional(?) or non-null(!) [choices: "none", "?", "!"] [default: "none"]
 * --index: Generate index file (boolean) [default: false]
 * --defaultExport: Generate index file (boolean) [default: false]
 */