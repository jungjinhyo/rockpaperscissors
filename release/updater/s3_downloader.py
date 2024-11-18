import boto3
import sys
import os

def download_file_from_s3(bucket_name, object_key, fullDownloadPath, credentials_file_path="aws_credentials.txt"):
    # 자격 증명 파일에서 액세스 키와 시크릿 키 읽기
    with open(credentials_file_path, 'r') as file:
        lines = file.readlines()
        access_key = lines[0].strip().split('=')[1]
        secret_key = lines[1].strip().split('=')[1]

    # boto3 세션 생성
    session = boto3.Session(
        aws_access_key_id=access_key,
        aws_secret_access_key=secret_key
    )
    s3 = session.client('s3')

    # S3에서 파일 다운로드
    try:
        s3.download_file(bucket_name, object_key, fullDownloadPath)
        print(f"Download successful: {object_key} to {fullDownloadPath}")
    except Exception as e:
        print(f"Download failed: {e}")

if __name__ == "__main__":
    # 명령줄 인수에서 S3 버킷 이름, 객체 키 및 다운로드 경로 가져오기
    if len(sys.argv) != 4:
        print("Usage: python s3_downloader.py <bucket_name> <object_key> <fullDownloadPath>")
        sys.exit(1)

    # 인자값 받아오기
    bucket_name = sys.argv[1]
    object_key = sys.argv[2]
    fullDownloadPath = sys.argv[3]

    # S3에서 파일 다운로드
    download_file_from_s3(bucket_name, object_key, fullDownloadPath)
