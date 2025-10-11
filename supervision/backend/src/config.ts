interface IApplicationConfig {
    albumPath: string;
    multipartFileTempPath: string; // Temporary path for receiving files
    exportPath: string;
    apiPublicUrl: string; // Optional, can be set in the environment
}

interface IExpressConfig {
    port: number;
}

interface IMysqlConfig {
    host: string;
    user: string;
    password: string;
    database: string;
    port: number;
    logging: boolean;
    charset: string;
}

interface IJwtConfig {
    secret: string;
    expiration: number;
}

interface IConfig {
    env: string;
    express: {
        port: number;
    };
    jwt: {
        secret: string;
        expiration: number;
    };
    mysql: {
        host: string;
        user: string;
        password: string;
        database: string;
        port: number;
        logging: boolean;
        charset: string;
    };

    application: {
        albumPath: string;
        apiPublicUrl?: string; // Optional, can be set in the environment
    };
}


export const expressConfig: IExpressConfig = {
    port: process.env.EXPRESS_PORT ? parseInt(process.env.EXPRESS_PORT) : 3000,
};

export const jwtConfig: IJwtConfig = {
    secret: process.env.JWT_SECRET || 'SECRET_KEY',
    expiration: process.env.JWT_EXPIRATION ? parseInt(process.env.JWT_EXPIRATION) : 7 // in days
}

export const mysqlConfig: IMysqlConfig = {
    host: process.env.MYSQL_HOST || 'MYSQL_HOST',
    user: process.env.MYSQL_USER || 'MYSQL_USER',
    password: process.env.MYSQL_PASSWORD || 'MYSQL_PASSWORD',
    database: process.env.MYSQL_DATABASE || 'MYSQL_DATABASE',
    port: Number(process.env.MYSQL_PORT) || 3306,
    logging: process.env.APP_ENV !== 'production' ? true : false,
    charset: 'utf8mb4'
}

export const applicationConfig: IApplicationConfig = {
    albumPath: process.env.ALBUMS_PATH || '/uploads/photos/',
    multipartFileTempPath: process.env.MULTIPART_FILE_TEMP_PATH || '/tmp/uploads/',
    exportPath: process.env.EXPORT_PATH || '/exports/', // Path for exported files
    apiPublicUrl: process.env.API_PUBLIC_URL || 'http://localhost:3000/api'
};

export const config: IConfig =  {
    env: process.env.APP_ENV || 'dev',

    express: expressConfig,

    jwt: jwtConfig,

    mysql: mysqlConfig,

    application: applicationConfig
};
