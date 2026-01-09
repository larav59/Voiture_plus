import { Column, Entity, OneToMany, PrimaryGeneratedColumn } from "typeorm";
import { Alarms } from "./Alarms";

@Entity("ORIGINS", { schema: "supervision" })
export class Origins {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("varchar", { name: "label", nullable: true, length: 255 })
  public label!: string | null;

  @OneToMany(() => Alarms, (alarms) => alarms.origin)
  public alarms!: Alarms[];

  public constructor(init?: Partial<Origins>) {
    Object.assign(this, init);
  }
}
